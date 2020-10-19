// -*- c++ -*-
#ifndef __SINGLETON_HH__
#define __SINGLETON_HH__

#include <stdexcept>
#include <sstream>
using namespace std;

template <class T> class Only {
private:
  static inline T* only{nullptr};
  
public:
  static T* get() {
    if (only == nullptr) {
      only = new T();
    }
    return only;
  }
  
  static void init(T* p)
  {
    if (only != nullptr) {
      ostringstream m;
      m << "Only::init: failed for " << typeid(T).name();
      throw runtime_error(m.str());
    }
    only = p;
  }
};

#endif
