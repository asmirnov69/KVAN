// -*- c++ -*-
#ifndef __KVAN_ENUM_IO_HH__
#define __KVAN_ENUM_IO_HH__

template <class T> string get_enum_value_string(T);
template <class T> void set_enum_value(T*, const string& new_v) {
  throw runtime_error(__func__);
}

#endif
