// -*- c++ -*-
#ifndef __KVAN_VECTOR_FJSON_IO__HH__
#define __KVAN_VECTOR_FJSON_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/struct-descriptor.h>
#include <kvan/csv-io.h>

class FJSONVisitor : public StructVisitor
{
public:
  ostream& out;
  FJSONVisitor(ostream& out_) : out(out_) {}
  void visit_null(const LOBKey& path) override;
  void visit_key(const LOBKey& path) override;
  void visit_enum(const LOBKey& path, const string& enum_s) override;
  void visit_string(const LOBKey& path, const string& s) override;
  void visit_fundamental(const LOBKey& path, const string& v) override;
  void visit_start_map(const LOBKey& path) override;
  void visit_end_map() override;
  void visit_delimiter() override;
  void visit_start_array() override;
  void visit_end_array() override;  
};

template <class T>
inline vector<string> to_fjson(ostream& out, const vector<T>& v)
{
  StructDescriptor sd = get_struct_descriptor<T>();

  CSVColumnsVisitor cols_v;
  LOBKey k;
  sd.visit_members(&cols_v, &k, v[0]);
  vector<string> columns = string_join(cols_v.cols, '.');
    
  out << "[";
  for (size_t i = 0; i < v.size(); i++) {
    FJSONVisitor fjson_v(out);
    LOBKey k;
    out << "{";
    sd.visit_members(&fjson_v, &k, v[i]);
    out << "}";
    if (i+1 < v.size()) out << ",";
  }
  out << "]";

  return columns;
}

#endif
