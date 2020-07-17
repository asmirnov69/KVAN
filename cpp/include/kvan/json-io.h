// -*- c++ -*-
#ifndef __KVAN_JSON_IO_HH__
#define __KVAN_JSON_IO_HH__

#include <iostream>
#include <any>
using namespace std;

#include <kvan/addl-type-traits.h>
#include <kvan/enum-io.h>
#include <kvan/struct-descriptor.h>

void to_json(ostream& out, const any& o, const StructDescriptor&);
vector<pair<string, string>> from_json(const string& json_s);

template <class T> inline
void to_json(ostream& out, const T& v)
{
  if constexpr(is_enum<T>::value) {
      out << "\"" + get_enum_value_string<T>(v) + "\"";
    } else if constexpr(is_string<T>::value) {
      out << "\"" + v + "\"";
    } else if constexpr(is_fundamental<T>::value) {
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
    } else if constexpr(is_function<decltype(get_struct_descriptor<T>)>::value) {
      auto sd = get_struct_descriptor<T>();
      to_json(out, v, sd);
    } else {
    throw runtime_error(__func__);
  }
}

template <class MT, class T>
inline void MemberDescriptorT<MT, T>::to_json__(ostream& out, const any& o) const
{
  try {
    out << "\"" << member_name << "\": ";
    const T& obj = any_cast<T>(o);
    const MT& member_v = obj.*mptr;
    to_json<MT>(out, member_v);
  } catch (const bad_any_cast& ex) {
    throw runtime_error(ex.what());      
  }
}

#endif
