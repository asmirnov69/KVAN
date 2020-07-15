// -*- c++ -*-
#ifndef __KVAN_VECTOR_JSON_IO__HH__
#define __KVAN_VECTOR_JSON_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/struct-descriptor.h>

template <class T>
inline string to_json(const vector<T>& v)
{
  StructDescriptor psd = get_struct_descriptor<T>();
  return "";
}

#endif
