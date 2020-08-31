// -*- c++ -*-
#ifndef __KVAN__TIME_UTILS_HH__
#define __KVAN__TIME_UTILS_HH__

#include <string>
using namespace std;

string to_isoformat(double utcts, bool microseconds = false);
double now_utctimestamp();

long long hires_ts_now();

#endif
