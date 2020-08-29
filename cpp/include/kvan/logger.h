// -*- c++ -*-
#ifndef __KVAN__LOGGER_HH__
#define __KVAN__LOGGER_HH__

#include <iostream>
#include <fstream>
#include <mutex>
using namespace std;

namespace kvan {
  class mt_stdout_streambuf : public streambuf
  {
  public:
    mt_stdout_streambuf(streambuf*);
    void set_logfile(ofstream*);
    
  protected:
    virtual int_type overflow(int_type c = traits_type::eof()) override;

  private:
    streambuf* sink = nullptr;
    ofstream* logfile = nullptr;
    std::mutex m_;
  };

  void logger_setup();
};

#endif
