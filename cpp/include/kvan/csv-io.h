// -*- c++ -*-
#ifndef __KVAN_VECTOR_CSV_IO__HH__
#define __KVAN_VECTOR_CSV_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/struct-descriptor.h>

class CSVColumnsVisitor : public StructVisitor
{
public:
  vector<LOBKey> cols;
  string get_header() {
    vector<string> cols_s = string_join(cols, '.');
    return string_join(cols_s, ',');
  }
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

class CSVVisitor : public StructVisitor
{
public:
  ostream& out;
  vector<LOBKey> cols;
  CSVVisitor(ostream& out_, const vector<LOBKey>& cols) : out(out_) {
    this->cols = cols;
  }
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

vector<string> parse_csv_line(const string& line);

template <class T> inline
void to_csv(ostream& out, const vector<T>& v)
{
  auto sd = get_struct_descriptor<T>();
  CSVColumnsVisitor cols_v;
  LOBKey k;
  sd.visit_members(&cols_v, &k, T());

  out << cols_v.get_header() << endl;
  for (auto& p: v) {
    CSVVisitor csv_v(out, cols_v.cols);
    LOBKey k;
    sd.visit_members(&csv_v, &k, p);
    out << endl;
  }
}

template <class T> inline
void from_csv(vector<T>* v, istream& in)
{
  auto sd = get_struct_descriptor<T>();
  
  string line;
  getline(in, line);
  vector<string> columns = string_split(line, ',');
  vector<LOBKey> lob_keys;
  for (auto& col: columns) {
    LOBKey k = {"?this"}; // first element of dpath
    auto cols = string_split(col, '.');
    k.insert(k.end(), cols.begin(), cols.end());
    lob_keys.emplace_back(k);
  }

  int line_no = 1;
  while (getline(in, line)) {
    vector<string> values = parse_csv_line(line);
    //cout << "csv line: " << string_join(values, ';') << endl;
    if (values.size() != columns.size()) {
      ostringstream m; m << "columns/values mismatch at line " << line_no;
      throw runtime_error(m.str());
    }
    T new_v;
    for (size_t i = 0; i < lob_keys.size(); i++) {
      auto lob_key = lob_keys[i];
      auto new_value = values[i];
      sd.set_value(&new_v, lob_key, new_value);
    }
    v->push_back(new_v);
    line_no++;
  }
}


#endif
