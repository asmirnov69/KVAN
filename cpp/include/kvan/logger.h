// -*- c++ -*-
#ifndef __KVAN__LOGGER_HH__
#define __KVAN__LOGGER_HH__

#include <iostream>
#include <mutex>
using namespace std;

#include <signal.h>

#include <kvan/fuargs.h>

namespace kvan {
  string get_log_fn(const char* argv0);

  void logger_setup(const char* argv0 = Fuargs::argv[0]);
  void sigio_setup(const char* argv0 = Fuargs::argv[0]);

  int write_fd = -1;
  int read_fd = -1;
  int logger_fd = -1;
  int saved_tty_fd = -1;
  void handler(int sig, siginfo_t *sinfo, void *value);

  class mt_stdout_streambuf : public streambuf
  {
  public:
    mt_stdout_streambuf(streambuf*);
    
  protected:
    virtual int_type overflow(int_type c = traits_type::eof()) override;

  private:
    streambuf* sink = nullptr;
    std::mutex m_;
  };

  
  
};

#endif
