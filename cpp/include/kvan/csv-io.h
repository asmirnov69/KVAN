// -*- c++ -*-
#ifndef __KVAN_VECTOR_CSV_IO__HH__
#define __KVAN_VECTOR_CSV_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/struct-descriptor.h>

vector<string> parse_csv_line(const string& line);

void to_csv_line(ostream&, const vector<LOBKey>& cols, const LOB& j);
void to_csv(ostream& out, const LOBVector& lobs);
void from_csv(LOBVector* lobs, istream& in);

#endif
