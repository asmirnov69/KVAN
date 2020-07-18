// -*- c++ -*-
#ifndef __KVAN_VECTOR_FJSON_IO__HH__
#define __KVAN_VECTOR_FJSON_IO__HH__

#include <iostream>
using namespace std;

#include <kvan/lob.h>

void to_fjson_line(ostream&, const LOB&);
void to_fjson(ostream& out, const vector<LOB>& lobs);

#endif
