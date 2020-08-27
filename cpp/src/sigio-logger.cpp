#include <iostream>
#include <stdexcept>
#include <sstream>
using namespace std;

#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <kvan/time-utils.h>
#include <kvan/sigio-logger.h>

int kvan::logger::write_fd = -1;
int kvan::logger::read_fd = -1;
int kvan::logger::logger_fd = -1;
int kvan::logger::saved_tty_fd = -1;

void kvan::logger::handler(int sig, siginfo_t *sinfo, void *value)
{
  char read_buf[PIPE_BUF];
  memset(read_buf, 0, sizeof read_buf);

  auto read_res = read(kvan::logger::read_fd, read_buf, PIPE_BUF);

  if (read_res > 0) {
    if (saved_tty_fd != -1) {
      write(saved_tty_fd, read_buf, read_res);
    }
    if (logger_fd != -1) {
      write(logger_fd, read_buf, read_res);
    }
  }
}

void kvan::logger::setup(const char* log_fn, bool output_to_console)
{
  logger_fd = ::open(log_fn, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  //cout << "logger_fd: " << logger_fd << endl;
  if (logger_fd == -1) {
    throw runtime_error("kvan::logger::setup: can't open log file");
  }

  ostringstream start_log_m;
  start_log_m << "=== " << to_isoformat(now_utctimestamp()) << " ";
  start_log_m << "pid: " << getpid() << '\n';
  write(logger_fd, start_log_m.str().c_str(), start_log_m.str().size());
  
  int fds[2];
  if (pipe(fds) != 0) {
    throw runtime_error("kvan::logger::setup: pipe() failed");
  }
  read_fd = fds[0];
  write_fd = fds[1];

  struct sigaction act;
  act.sa_sigaction = handler;
  act.sa_flags = 0;
  act.sa_flags = SA_RESTART;
  sigemptyset(&act.sa_mask);
  if (sigaction(SIGIO, &act, NULL) == -1) {
    throw runtime_error("kvan::logger::setup: attempt to set up handler for SIGIO failed");
  }

    /* arrange to get the signal */
  if (fcntl(read_fd, F_SETOWN, getpid()) == -1) {
    throw runtime_error("kvan::logger::setup: fnctl to set F_SETOWN failed");
  }
  auto flags = fcntl(read_fd, F_GETFL);
  if (flags >= 0 && fcntl(read_fd, F_SETFL, flags | O_ASYNC ) == -1)
    throw runtime_error("kvan::logger::setup: fnctl F_SETFL to set O_ASYNC failed");

  if (output_to_console) {
    saved_tty_fd = dup(1);
  }
  dup2(write_fd, 1);
  dup2(write_fd, 2);
}
