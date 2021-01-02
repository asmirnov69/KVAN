// -*- c++ -*-
#ifndef __CBQ_HH__
#define __CBQ_HH__

//
// using example from "Programming With Threads" by Kleiman et al, pp. 36-38
// as guideline

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <iostream>
#include <array>
using namespace std;
using namespace std::chrono_literals;

template <class T, int QSIZE> struct CBQ // curcular buffer queue
{
private:  
  array<T, QSIZE> data;
  int start_idx{0}, num_full{0};
  mutex buf_lock;
  condition_variable notfull, notempty;

public:
  CBQ() {}
  CBQ& operator=(const CBQ& o) {
    data = o.data;
    start_idx = o.start_idx;
    num_full = o.num_full;
    return *this;
  }
  void blocking_put(const T& in_data) {
    unique_lock<mutex> l(buf_lock);

    while(num_full == QSIZE) {
      notfull.wait(l);
    }

    data[(start_idx + num_full) % QSIZE] = in_data;
    num_full += 1;
    notempty.notify_one();
  }

  bool nonblocking_put(const T& in_data) {
    unique_lock<mutex> l(buf_lock);

    if (num_full == QSIZE) {
      //notfull.wait(buf_lock);
      return false; // queue is full, put has no effect
    }

    data[(start_idx + num_full) % QSIZE] = in_data;
    num_full += 1;
    notempty.notify_one();
    return true;
  }
  
  void blocking_get(T* out_data) {
    unique_lock<mutex> l(buf_lock);

    while (num_full == 0) {
      notempty.wait(l);
    }

    *out_data = data[start_idx];
    start_idx= (start_idx + 1) % QSIZE;
    num_full -= 1;

    notfull.notify_one();
  }

  bool nonblocking_get(T* out_data) {
    unique_lock<mutex> l(buf_lock);

    if (num_full == 0) {
      //notempty.wait(buf_lock);
      return false;
    }

    *out_data = data[start_idx];
    start_idx= (start_idx + 1) % QSIZE;
    num_full -= 1;

    notfull.notify_one();
    return true;
  }
};

#endif
