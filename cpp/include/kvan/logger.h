// -*- c++ -*-
#ifndef __KVAN__LOGGER_HH__
#define __KVAN__LOGGER_HH__

#include <iostream>
#include <mutex>
using namespace std;

#include <signal.h>

namespace kvan {
  string get_log_fn(const char* argv0);
  void log_annotation_setup();
  void logger_setup(const char* argv0);

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
