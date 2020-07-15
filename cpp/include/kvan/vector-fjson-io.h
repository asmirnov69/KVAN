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
  vector<string> columns;
  psd.get_columns(&columns);

  string out;
  out += "[";
  for (size_t i = 0; i < v.size(); i++) {
    vector<ValuePathValue> p_json;
    psd.get_value(v[i], &p_json);
    out += to_fjson_string(p_json);
    if (i+1 < v.size()) out += ",";
  }
  out += "]";
  
  return make_pair(columns, out);
}

#endif
