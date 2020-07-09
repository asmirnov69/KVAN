// -*- c++ -*-
#ifndef __SINGLETON_HH__
#define __SINGLETON_HH__

template <class T> class Only {
public:
  static T* get() {
    static T* only = 0;
    if (only == nullptr) {
      only = new T();
    }
    return only;
  }
};

#endif
