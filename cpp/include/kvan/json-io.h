// -*- c++ -*-
#ifndef __KVAN_JSON_IO_HH__
#define __KVAN_JSON_IO_HH__

#include <iostream>
#include <any>
using namespace std;

#include <kvan/addl-type-traits.h>
#include <kvan/enum-io.h>
#include <kvan/struct-descriptor.h>

class JSONVisitor : public StructVisitor
{
public:
  ostringstream out;
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

vector<pair<string, string>> from_json(const string& json_s);

template <class T>
inline void from_json(T* obj, const string& json_s)
{
  auto sd = get_struct_descriptor<T>();

  auto values = from_json(json_s);
  for (auto& v: values) {
    sd.set_value(obj, string_split(v.first, '.'), v.second);
  }
}

template <class T>
inline void to_json(ostream& out, const T& obj)
{
  auto sd = get_struct_descriptor<T>();
  JSONVisitor json_visitor;
  LOBKey k;
  sd.visit_members(&json_visitor, &k, obj);    
  out << json_visitor.out.str() << endl;
}

#endif
