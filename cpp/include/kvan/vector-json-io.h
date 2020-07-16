// -*- c++ -*-
#ifndef __KVAN_VECTOR_JSON_IO__HH__
#define __KVAN_VECTOR_JSON_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/struct-descriptor.h>

template <class T>
inline void to_json(ostream& out, const T& v)
{
  if constexpr(is_fundamental<T>::value) {
      out << v;
    } else if constexpr(is_vector<T>::value) {
      out << "[";
      for (size_t i = 0; i < v.size(); ++i) {
	if constexpr(is_fundamental<typename T::value_type>::value) {
	  out << v[i];
	} else {
	  to_json<typename T::value_type>(out, v[i]);
	}
	if (i + 1 < v.size()) {
	  out << ", ";
	}
      }
      out << "]";
    } else {
    StructDescriptor psd = get_struct_descriptor<T>();
    psd.to_json(out, v);
    }  
}

#endif
