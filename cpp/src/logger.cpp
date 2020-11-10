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

namespace kvan {
  int logger_fd = -1;
  int saved_tty_fd = -1;
}

string kvan::get_log_fn(const char* argv0)
{
  fs::path logdir = fs::path(getenv("HOME")) / "log";
  if (!fs::exists(logdir)) {
    if (!fs::create_directories(logdir)) {
      throw runtime_error("kvan::logger_setup: can't create log dir " + logdir.string());
    }
  }

  auto logfile_fn = fs::path(argv0).filename().string() + "." + to_string(getpid()) + ".log";
  return (logdir / logfile_fn).string();
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
    os.flush();
    line_buf.str("");
    line_buf.clear();
  }
  return traits_type::to_int_type(c);
}

void kvan::log_annotation_setup()
{
  auto cout_a_b = new kvan::mt_stdout_streambuf(cout.rdbuf());
  auto cerr_a_b = new kvan::mt_stdout_streambuf(cerr.rdbuf());
  cout.rdbuf(cout_a_b);
  cerr.rdbuf(cerr_a_b);
}

void kvan::logger_setup(const char* argv0)
{
  auto log_fn = get_log_fn(argv0);
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
  
  saved_tty_fd = dup(1);
  dup2(logger_fd, 1);
  dup2(logger_fd, 2);
}
