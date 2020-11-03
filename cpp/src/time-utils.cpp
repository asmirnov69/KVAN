#include <string>
#include <chrono>
#include <cmath>
using namespace std;

#include <kvan/time-utils.h>

double now_utctimestamp()
{
  // from https://stackoverflow.com/a/34781413/1181482
  using clock = std::chrono::high_resolution_clock;
  using dsec = std::chrono::duration<double>;
  using tps = std::chrono::time_point<clock, dsec>;
  tps tp = clock::now();
  return tp.time_since_epoch().count();
}

std::string to_isoformat(double utcts, bool microseconds)
{
  struct tm localtime_res_o;
  time_t rawtime = utcts;
  auto localtime_res = localtime_r(&rawtime, &localtime_res_o);
  if (localtime_res) {
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", localtime_res);
    string ret(buf);

    if (microseconds) {
      double dd;
      double microsecs = modf(utcts, &dd);
      ret += to_string(microsecs).substr(1);
    }
    return ret;
  }
  
  return "::nant::";
}

long long hires_ts_now()
{
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

  return nanoseconds.count();
}

#if 0 // old implementation
{
  // from https://stackoverflow.com/a/34781413/1181482
  using clock = std::chrono::high_resolution_clock;
  using dsec = std::chrono::duration<double, std::milli>;
  using tps = std::chrono::time_point<clock, dsec>;
  tps tp = clock::now();
  return tp.time_since_epoch().count();
}
#endif

double to_utctimestamp(const char* t, const char* fmt)
{
  struct tm tm;
  //cout << "tm.isdst: " << tm.tm_isdst << endl;
  strptime(t, fmt, &tm);
  tm.tm_isdst = -1; // nasty, explained at https://stackoverflow.com/a/24185697/1181482
  time_t res = mktime(&tm);
  return res;
}

std::string format_utctimestamp(double utcts, const char* fmt)
{
  time_t rawtime = utcts;
  struct tm ts;
  char buf[80];
  
  // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
  auto localtime_res = localtime(&rawtime);
  if (localtime_res) {
    ts = *localtime_res;
    strftime(buf, sizeof(buf), fmt, &ts);
    string ret(buf);

    double dd;
    double millisecs = modf(utcts, &dd);
    ret += to_string(millisecs).substr(1);
    return ret;
  }
  
  return "::nant::";
}
