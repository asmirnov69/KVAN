// -*- c++ -*-
#ifndef __KVAN_VECTOR_CSV_IO__HH__
#define __KVAN_VECTOR_CSV_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/struct-descriptor.h>

string to_csv_string(const JKV& j);

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

template <class T>
inline void from_csv(vector<T>* v, istream& in)
{
  auto sd = get_struct_descriptor<T>();

  string line;
  getline(in, line);
  vector<string> columns = string_split(line, ',');
  int line_no = 1;

  while (getline(in, line)) {
    vector<string> values = parse_csv_line(line);
    //cout << "csv line: " << string_join(values, ';') << endl;
    if (values.size() != columns.size()) {
      ostringstream m; m << "columns/values mismatch at line " << line_no;
      throw runtime_error(m.str());
    }
    T new_v;
    for (size_t i = 0; i < columns.size(); i++) {
      auto col_name = columns[i];
      auto new_value = values[i];
      //cout << "set: col_name: " << col_name << ", new_value: " << new_value
      // << endl;
      sd.set_value(&new_v, col_name, new_value);
    }
    v->push_back(new_v);
    line_no++;
  }
}

#endif
