// -*- c++ -*-
#ifndef __KVAN_VECTOR_FJSON_IO__HH__
#define __KVAN_VECTOR_FJSON_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/struct-descriptor.h>

string to_fjson_string(const vector<ValuePathValue>& j);

template <class T>
inline ValuePathValue to_fjson(const vector<T>& v)
{
  StructDescriptor psd = get_struct_descriptor<T>();  
  auto vps = psd.get_value_pathes();
  vector<string> columns = string_join(vps, '.');

  string out;
  out += "[";
  for (size_t i = 0; i < v.size(); i++) {
    auto p_json = psd.get_values(v[i]);
    out += to_fjson_string(p_json);
    if (i+1 < v.size()) out += ",";
  }
  out += "]";
  
  return make_pair(columns, out);
}

#endif
