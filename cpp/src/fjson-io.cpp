#include <sstream>
using namespace std;

#include <kvan/string-utils.h>
#include <kvan/fjson-io.h>

void to_fjson_line(ostream& out, const LOB& l)
{
  out << "{";
  for (auto it = l.begin(); it != l.end(); ++it) {
    out << "\"" << string_join((*it).first, '.') << "\"";
    out << ": " << (*it).second;
    if (next(it) != l.end()) {
      out << ", ";
    }
  }
  out << "}";
}

void to_fjson(ostream& out, const vector<LOB>& v)
{
  out << "[";
  for (size_t i = 0; i < v.size(); i++) {
    to_fjson_line(out, v[i]);
    if (i+1 < v.size()) out << ",";
  }
  out << "]";
}

