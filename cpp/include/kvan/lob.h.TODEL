// -*- c++ -*-
#ifndef __KVAN_LOB__HH__
#define __KVAN_LOB__HH__

#include <vector>
#include <string>
#include <map>
using namespace std;

// Linear OBject -- LOB (unlike hirerhical objects like in c++ etc)
// LOB is set of key,value pairs where key is list of strings.

typedef vector<string> LOBKey;
typedef pair<LOBKey, string> LOBKeyValue; // value path -> value

class LOB
{
public:
  vector<LOBKeyValue> kvs;
  map<LOBKey, size_t> kvs_index;
  
public:
  explicit LOB();
  void set(const vector<LOBKeyValue>& kvs);

  bool has(const LOBKey& k) const;
  string get(const LOBKey& k) const;
  void set(const LOBKey& k, const string& v);
  
  vector<LOBKeyValue>::iterator begin() { return kvs.begin(); }
  vector<LOBKeyValue>::const_iterator begin() const { return kvs.begin(); }
  vector<LOBKeyValue>::iterator end() { return kvs.end(); }
  vector<LOBKeyValue>::const_iterator end() const { return kvs.end(); }
};

class LOBVector
{
public:
  vector<LOBKey> keys;
  vector<LOB> lobs;

  explicit LOBVector() {}  
  void init(const vector<LOBKey>& keys) {this->keys = keys;this->lobs.clear(); }
  void push_back(const LOB& l) { lobs.push_back(l); }

  vector<LOB>::iterator begin() { return lobs.begin(); }
  vector<LOB>::iterator end() { return lobs.end(); }
};

#endif
