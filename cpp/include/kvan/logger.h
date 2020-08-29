// -*- c++ -*-
#ifndef __KVAN__LOGGER_HH__
#define __KVAN__LOGGER_HH__

#include <iostream>
#include <mutex>
using namespace std;

namespace kvan {
  class mt_stdout_streambuf : public streambuf
  {
  public:
    mt_stdout_streambuf(streambuf*);
  
  protected:
    virtual int_type overflow(int_type c = traits_type::eof()) override;

  private:
    streambuf* sink_;
    std::mutex m_;    
  };

  void logger_setup();
};

#endif
