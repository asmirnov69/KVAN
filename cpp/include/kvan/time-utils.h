// -*- c++ -*-
#ifndef __KVAN__TIME_UTILS_HH__
#define __KVAN__TIME_UTILS_HH__

#include <string>
using namespace std;

string to_isoformat(double utcts, bool microseconds = false);
double now_utctimestamp();
double to_utctimestamp(const char* t,
		       const char* fmt = "%Y-%m-%d %H:%M:%S");
double to_utctimestamp(const char* t_date, const char* t_date_fmt,
		       const char* t_time, const char* t_time_fmt);
string format_utctimestamp(double utcts,
			   const char* fmt = "%Y-%m-%d %H:%M:%S",
			   bool add_sec_fractions = false);

long long hires_ts_now();

#endif
