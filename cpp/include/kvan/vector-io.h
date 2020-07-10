// -*- c++ -*-
#ifndef __KVAN_VECTOR_IO__HH__
#define __KVAN_VECTOR_IO__HH__

#include <kvan/struct-descriptor.h>

inline string to_json_string(const JKV& j)
{
  ostringstream out;
  out << "{";
  for (auto it = j.begin(); it != j.end(); ++it) {
    const auto& [k, v] = *it;
    out << "\"";
    for (auto kit = k.begin(); kit != k.end(); ++kit) {
      out << (*kit);
      if (next(kit) != k.end()) {
	out << ".";
      }
    }
    out << "\"";
    out << ": " << v;
    if (next(it) != j.end()) {
      out << ", ";
    }
  }
  out << "}";
  return out.str();
}

template <class T>
inline pair<vector<string>, string> to_json_dataframe(const vector<T>& v)
{
  StructDescriptor psd = get_struct_descriptor<T>();  
  vector<string> columns;
  psd.get_columns(&columns);

  string out;
  out += "[";
  for (size_t i = 0; i < v.size(); i++) {
    JKV p_json;
    psd.get_value(v[i], &p_json);
    out += to_json_string(p_json);
    if (i+1 < v.size()) out += ",";
  }
  out += "]";
  
  return make_pair(columns, out);
}

inline string to_csv_string(const JKV& j)
{
  ostringstream out;
  for (auto it = j.begin(); it != j.end(); ++it) {
    const auto& [k, v] = *it;
    out << v;
    if (next(it) != j.end()) {
      out << ",";
    }
  }
  return out.str();
}

template <class T>
inline void to_csv(ostream& out, const vector<T>& v)
{
  StructDescriptor psd = get_struct_descriptor<T>();  
  vector<string> columns;
  psd.get_columns(&columns);

  for (size_t i = 0; i < columns.size(); i++) {
    out << columns[i];
    if (i + 1 < columns.size()) {
      out << ",";
    }
  }
  out << endl;

  for (size_t i = 0; i < v.size(); i++) {
    JKV p_json;
    psd.get_value(v[i], &p_json);
    out << to_csv_string(p_json);
    out << endl;
  }
}


#endif
