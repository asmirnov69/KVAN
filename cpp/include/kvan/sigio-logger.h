// -*- c++ -*-
#ifndef __SIGIO_LOGGER__HH__
#define __SIGIO_LOGGER__HH__

#include <signal.h>

namespace kvan {
  class logger {
  public:
    static void setup(const char* log_fn);
  private:
    static int read_fd, write_fd;
    static int saved_tty_fd, logger_fd;
    static void handler(int sig, ::siginfo_t* sinfo, void *value);
  };
}

#endif
