#include <sstream>
using namespace std;

#include <unistd.h>
#include <kvan/logger.h>
#include <kvan/time-utils.h>

void kvan::logger_setup()
{
  auto nb = new kvan::mt_stdout_streambuf(cout.rdbuf());
  cout.rdbuf(nb);
}

kvan::mt_stdout_streambuf::mt_stdout_streambuf(streambuf* sbuf)
{
  this->sink_ = sbuf;
}

streambuf::int_type kvan::mt_stdout_streambuf::overflow(int_type c)
{
  if (traits_type::eq_int_type(c, traits_type::eof())) {
    return sink_->pubsync() == -1 ? c : traits_type::not_eof(c);
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
    std::ostream os(sink_);
    std::unique_lock<std::mutex> lock(m_);
    os << line_buf.str();
    line_buf.str("");
    line_buf.clear();
  }
  return traits_type::to_int_type(c);
}
