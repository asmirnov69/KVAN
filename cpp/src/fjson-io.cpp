#include <sstream>
using namespace std;

#include <kvan/string-utils.h>
#include <kvan/fjson-io.h>

void FJSONVisitor::visit_enum(const LOBKey& path, const string& enum_s)
{
  out << "\"" << string_join(path, '.') << "\": \"" << enum_s << "\"";
}

void FJSONVisitor::visit_string(const LOBKey& path, const string& s)
{
  out << "\"" << string_join(path, '.') << "\": \"" << s << "\"";
}

void FJSONVisitor::visit_fundamental(const LOBKey& path, const string& v)
{
  out << "\"" << string_join(path, '.') << "\": " << v;
}

void FJSONVisitor::visit_start_map(const LOBKey& path)
{
}
void FJSONVisitor::visit_end_map()
{
}

void FJSONVisitor::visit_delimiter()
{
  out << ", ";
}
void FJSONVisitor::visit_start_array()
{
  throw runtime_error("not implemented");
}

void FJSONVisitor::visit_end_array()
{
  throw runtime_error("not implemented");
}

