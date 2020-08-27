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
