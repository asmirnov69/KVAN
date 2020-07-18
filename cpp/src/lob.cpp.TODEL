#include <sstream>
using namespace std;

#include <kvan/lob.h>
#include <kvan/string-utils.h>

LOB::LOB()
{
}

void LOB::set(const vector<LOBKeyValue>& kvs)
{
  this->kvs = kvs;
  this->kvs_index.clear();
  for (size_t i = 0; i < this->kvs.size(); i++) {
    this->kvs_index[kvs[i].first] = i;
  }
}

bool LOB::has(const LOBKey& k) const
{
  return this->kvs_index.find(k) != this->kvs_index.end();
}

string LOB::get(const LOBKey& k) const
{
  auto it = this->kvs_index.find(k);
  if (it == this->kvs_index.end()) {
    ostringstream m; m << "LOB::get: key " << string_join(k, '.') << " not found";
    throw runtime_error(m.str());
  }
  return this->kvs[(*it).second].second;
}

void LOB::set(const LOBKey& k, const string& v)
{
  auto it = this->kvs_index.find(k);
  if (it == this->kvs_index.end()) {
    this->kvs_index[k] = this->kvs.size();
    this->kvs.push_back(make_pair(k, v));
  } else {
    this->kvs[(*it).second].second = v;
  }
}

