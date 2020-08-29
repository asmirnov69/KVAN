#include <sstream>
#include <cstdlib>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <unistd.h>
#include <kvan/logger.h>
#include <kvan/fuargs.h>
#include <kvan/time-utils.h>


static string get_log_fn(const char* argv0)
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
  
void kvan::mt_stdout_streambuf::set_logfile(ofstream* o)
{
  this->logfile = o;
}

void kvan::logger_setup()
{
  auto log_fn = get_log_fn(Fuargs::argv[0]);
  auto cout_a_b = new kvan::mt_stdout_streambuf(cout.rdbuf());
  auto cerr_a_b = new kvan::mt_stdout_streambuf(cerr.rdbuf());
  cout.rdbuf(cout_a_b);
  cerr.rdbuf(cerr_a_b);

  auto logfile = new ofstream(log_fn, ios::app);
  cout_a_b->set_logfile(logfile);
  cerr_a_b->set_logfile(logfile);
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
    if (logfile) {
      (*logfile) << line_buf.str();
      logfile->flush();
    }
    line_buf.str("");
    line_buf.clear();
  }
  return traits_type::to_int_type(c);
}
