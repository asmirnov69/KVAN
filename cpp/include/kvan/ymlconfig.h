// -*- c++ -*-
#ifndef __KVAN__YML_CONFIG_HH__
#define __KVAN__YML_CONFIG_HH__

#include <string>
#include <vector>
#include <map>
using namespace std;

#include <yaml.h>

class YMLConfigPath
{
private:
  friend class YMLConfig;
  friend bool operator<(const YMLConfigPath& l, const YMLConfigPath& r);
  vector<pair<string, int>> path;

public:
  YMLConfigPath();
  YMLConfigPath(const vector<string>& curr_path, const vector<int>& curr_index);
  bool is_subpath_of(const YMLConfigPath&);
  void from_string(const string&);
  string to_string() const;  
};

bool operator<(const YMLConfigPath&, const YMLConfigPath&);

class YMLConfig {
 private:
  vector<string> curr_path;
  vector<int> curr_index;
  enum { INIT, KEY, VALUE } state{INIT};
  
  vector<pair<YMLConfigPath, string>> values;
  map<YMLConfigPath, string> values_map;
  bool debug = false;
  bool dry_run = false;
  bool enable_dollar_var_expansion = true;
  string stem;
  
  void handle_YAML_BLOCK_MAPPING_START_TOKEN(yaml_parser_t*);
  void handle_YAML_BLOCK_SEQUENCE_START_TOKEN(yaml_parser_t*);
  
public:
  explicit YMLConfig(const string& stem = "",
		     bool enable_dollar_var_expansion = true,
		     bool debug = false, bool dry_run = false);
  
  void parse(const char* yml_fn, const vector<string>& pp_pathes);
  std::string get(const char* path) const;
  YMLConfig get_config(const char* path) const;  
  void dump() const;
};

#endif
