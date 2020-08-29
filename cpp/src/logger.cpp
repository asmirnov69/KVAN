#include <sstream>
#include <cstdlib>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>

#include <kvan/logger.h>
#include <kvan/fuargs.h>
#include <kvan/time-utils.h>

string kvan::get_log_fn(const char* argv0)
{
  fs::path logdir = fs::path(getenv("HOME")) / "log";
  if (!fs::exists(logdir)) {
    if (!fs::create_directories(logdir)) {
      throw runtime_error("kvan::logger_setup: can't create log dir " + logdir.string());
    }
  }

  auto logfile_fn = fs::path(Fuargs::argv[0]).filename().string() + "." + to_string(getpid()) + ".log";
  return (logdir / logfile_fn).string();
}
  
void kvan::logger_setup()
{
  auto log_fn = get_log_fn(Fuargs::argv[0]);
  auto cout_a_b = new kvan::mt_stdout_streambuf(cout.rdbuf());
  auto cerr_a_b = new kvan::mt_stdout_streambuf(cerr.rdbuf());
  cout.rdbuf(cout_a_b);
  cerr.rdbuf(cerr_a_b);
}

kvan::mt_stdout_streambuf::mt_stdout_streambuf(streambuf* sbuf)
{
  this->sink = sbuf;
}

streambuf::int_type kvan::mt_stdout_streambuf::overflow(int_type c)
{
  if (traits_type::eq_int_type(c, traits_type::eof())) {
    return sink->pubsync() == -1 ? c : traits_type::not_eof(c);
  }
  
  thread_local bool nl{true};
  thread_local ostringstream line_buf;
  //tid_t tid = std::this_thread::get_id();

  if (nl) {      
     if (!(line_buf << to_isoformat(now_utctimestamp()) << "[" << getpid() << "]: ")) {
      return traits_type::eof();
    }
  }
    
  line_buf << traits_type::to_char_type(c);
  nl = traits_type::to_char_type(c) == '\n';
  if (nl) {
    std::ostream os(sink);
    std::unique_lock<std::mutex> lock(m_);
    os << line_buf.str();
    line_buf.str("");
    line_buf.clear();
  }
  return traits_type::to_int_type(c);
}

// .....................................................

void kvan::handler(int sig, siginfo_t *sinfo, void *value)
{
  char read_buf[PIPE_BUF];
  ssize_t read_res = 0;

  do {
    read_res = read(read_fd, read_buf, PIPE_BUF);
    if (read_res > 0) {
      if (saved_tty_fd != -1) {
	write(saved_tty_fd, read_buf, read_res);
      }
      if (logger_fd != -1) {
	write(logger_fd, read_buf, read_res);
      }
    }
  } while (read_res > 0);
}

static void set_fd_flags(int fd)
{
  if (fcntl(fd, F_SETSIG, SIGIO) == -1) {
    throw runtime_error("kvan::logger::setup: fnctl to set F_SETSIG failed");
  }
  
  if (fcntl(fd, F_SETOWN, getpid()) == -1) {
    throw runtime_error("kvan::logger::setup: fnctl to set F_SETOWN failed");
  }

  auto flags = fcntl(fd, F_GETFL);
  if (flags >= 0 && fcntl(fd, F_SETFL, flags | O_ASYNC ) == -1)
    throw runtime_error("kvan::logger::setup: fnctl F_SETFL to set O_ASYNC failed");
}

void kvan::sigio_setup()
{
  auto log_fn = get_log_fn(Fuargs::argv[0]);
  logger_fd = ::open(log_fn.c_str(),
		     O_CREAT|O_WRONLY|O_APPEND,
		     S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  //cout << "logger_fd: " << logger_fd << endl;
  if (logger_fd == -1) {
    throw runtime_error("kvan::logger::setup: can't open log file");
  }

  ostringstream start_log_m;
  start_log_m << "\n=== " << to_isoformat(now_utctimestamp()) << " ";
  start_log_m << "pid: " << getpid() << '\n';
  write(logger_fd, start_log_m.str().c_str(), start_log_m.str().size());

  on_exit([](int e_code, void*) {
      ostringstream m;
      m << "--- " << to_isoformat(now_utctimestamp()) << " ";
      m << "pid: " << getpid() << " exit code " << e_code << endl;
      write(kvan::logger_fd, m.str().c_str(), m.str().size());
    },
    0);
    
  int fds[2];
  if (pipe2(fds, O_DIRECT | O_NONBLOCK) != 0) {
    throw runtime_error("kvan::logger::setup: pipe2() failed");
  }
  read_fd = fds[0]; write_fd = fds[1];

  struct sigaction act;
  act.sa_sigaction = handler;
  act.sa_flags = 0;
  act.sa_flags = SA_SIGINFO;
  sigemptyset(&act.sa_mask);
  if (sigaction(SIGIO, &act, NULL) == -1) {
    throw runtime_error("kvan::logger::setup: attempt to set up handler for SIGIO failed");
  }

  /* arrange to get the signal */
  set_fd_flags(read_fd);

  saved_tty_fd = dup(1);
  dup2(write_fd, 1);
  dup2(write_fd, 2);
}
