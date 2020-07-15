// -*- c++ -*-
#ifndef __KVAN_VECTOR_JSON_IO__HH__
#define __KVAN_VECTOR_JSON_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/struct-descriptor.h>

template <class T>
inline void to_json(ostream& out, const T& v)
{
  StructDescriptor psd = get_struct_descriptor<T>();
  psd.to_json(out, v);
}

template <class T>
inline void to_json(ostream& out, const vector<T>& v)
{
  StructDescriptor psd = get_struct_descriptor<T>();
  out << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    psd.to_json(out, v[i]);
    if (i + 1 < v.size()) {
      out << ", ";
    }
  }
  out << "]";
}

#endif
