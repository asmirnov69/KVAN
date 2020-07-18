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


#endif
