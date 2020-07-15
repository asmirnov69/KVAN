#include <kvan/vector-csv-io.h>

string to_csv_string(const JKV& j)
{
  ostringstream out;
  for (auto it = j.begin(); it != j.end(); ++it) {
    const auto& [k, v] = *it;
    out << v;
    if (next(it) != j.end()) {
      out << ",";
    }
  }
  return out.str();
}

