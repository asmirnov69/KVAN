#include <sstream>
#include <stdexcept>
#include <iostream>
using namespace std;

#include <yaml.h>

#include <kvan/ymlconfig.h>
#include <kvan/ymlconfig-pp.h>
#include <kvan/string-utils.h>

bool operator<(const YMLConfigPath& l, const YMLConfigPath& r)
{
  return l.to_string() < r.to_string();
}

YMLConfigPath::YMLConfigPath(const vector<string>& curr_path,
			     const vector<int>& curr_index)
{
  path.push_back(make_pair("", -1));

  if (curr_path.size() != curr_index.size()) {
    throw runtime_error("YMLConfigPath::YMLConfigPath: path and index size mismatch");
  }
  for (size_t i = 0; i < curr_path.size(); i++) {
    this->path.push_back(make_pair(curr_path[i], curr_index[i]));
  }
}

YMLConfigPath::YMLConfigPath()
{
  path.push_back(make_pair("", -1));
}

bool YMLConfigPath::is_subpath_of(const YMLConfigPath& p)
{
  bool ret = false;
  if (this->path.size() <= p.path.size()) {
    ret = true;
    for (size_t i = 0; i < this->path.size(); i++) {
      if (!((p.path[i].first == this->path[i].first)
	    &&
	    (this->path[i].second == -1 || p.path[i].second == this->path[i].second)
	    )) {
	ret = false;
	break;
      }
    }
  }

  //cout << "is_subpath:" << this->to_string() << " " << p.to_string()
  //     << " --> " << ret << endl;
  return ret;
}

void YMLConfigPath::from_string(const string& path_s)
{
  istringstream i(path_s);
  string token;
  while (getline(i, token, '.')) {
    if (token[0] == '[') {
      int idx = atoi(token.substr(1, token.size() - 2).c_str());
      path[path.size() - 1].second = idx;
    } else {
      path.push_back(make_pair(token, -1));
    }
  }
}

string YMLConfigPath::to_string() const
{
  ostringstream o;
  for (size_t i = 0; i < path.size(); i++) {
    o << path[i].first;
    if (path[i].second >= 0) {
      o << ".[" << path[i].second << "]";
    }
    if (i+1 < path.size()) {
      o << ".";
    }
  }
  return o.str();
}

YMLConfig::YMLConfig(bool debug, bool dry_run)
{
  this->debug = debug;
  this->dry_run = dry_run;
  this->pp_pathes = {".", evaluate_dollar_var_expr("${etc-dir}")};
}

void YMLConfig::set_pp_pathes(const vector<string>& pp_pathes)
{
  this->pp_pathes = pp_pathes;
}

void YMLConfig::handle_YAML_BLOCK_MAPPING_START_TOKEN(yaml_parser_t* parser)
{
  this->curr_path.push_back("");
  this->curr_index.push_back(-1);
  
  bool done = false;
  yaml_token_t token;
  do {
    yaml_parser_scan(parser, &token);
    switch (token.type) {
    case YAML_KEY_TOKEN:
      if (debug) cout << "YAML_KEY_TOKEN" << endl;
      if (!dry_run) {
	this->state = KEY;
      }
      break;
    case YAML_VALUE_TOKEN:
      if (debug) cout << "YAML_VALUE_TOKEN" << endl;
      if (!dry_run) {
	this->state = VALUE;
      }
      break;
    case YAML_SCALAR_TOKEN:
      if (debug) {
	cout << "YAML_SCALAR_TOKEN" << endl;
	cout << " scalar " << token.data.scalar.value << endl;
	cout << " path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }
      if (!dry_run) {
	if (this->state == KEY) {
	  curr_path[curr_path.size()-1] = string((char*)token.data.scalar.value);
	} else if (this->state == VALUE) {
	  YMLConfigPath p(curr_path, curr_index);
	  string v((char*)token.data.scalar.value);
	  if (this->enable_dollar_var_expansion) {
	    v = evaluate_dollar_var_expr(v);
	  }
	  this->values.push_back(make_pair(p, v));
	} else {
	  throw runtime_error("YMLConfig::handle_YAML_BLOCK_MAPPING_START_TOKEN: unknown state");
	}
	this->state = INIT;
      }
      break;
    case YAML_BLOCK_MAPPING_START_TOKEN:
      if (debug) {
	cout << "YAML_BLOCK_MAPPING_START_TOKEN" << endl;
	cout << " path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }      
      handle_YAML_BLOCK_MAPPING_START_TOKEN(parser);
      break;
    case YAML_BLOCK_SEQUENCE_START_TOKEN:      
      if (debug) {
	cout << "YAML_BLOCK_SEQUENCE_START_TOKEN" << endl;
	cout << " path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }
	  handle_YAML_BLOCK_SEQUENCE_START_TOKEN(parser);
	break;
    case YAML_BLOCK_END_TOKEN:
      if (debug) {
	cout << "YAML_BLOCK_END_TOKEN" << endl;
	cout << "path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }
      if (!dry_run) {
	curr_index.pop_back();
	curr_path.pop_back();	
      }
      done = true;
      break;
    default:
      if (debug) {
	cout << "handle_YAML_BLOCK_MAPPING_START_TOKEN: unexpected token " << token.type << endl;
      }
      if (!dry_run) {
	ostringstream m;
	m << "handle_YAML_BLOCK_MAPPING_START_TOKEN: unexpected token " << token.type << endl
	  << "path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
					     throw runtime_error(m.str());
      }
      
      break;
    }

    yaml_token_delete(&token);
  } while (!done);
}

void YMLConfig::handle_YAML_BLOCK_SEQUENCE_START_TOKEN(yaml_parser_t* parser)
{
  bool done = false;
  yaml_token_t token;
  do {
    yaml_parser_scan(parser, &token);

    switch (token.type) {
    case YAML_BLOCK_ENTRY_TOKEN:
      if (debug) {
	cout << "YAML_BLOCK_ENTRY_TOKEN" << endl;
	cout << "path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }
      if (!dry_run) {
	curr_index[curr_index.size() - 1] += 1;
      }
      break;
    case YAML_BLOCK_END_TOKEN:
      if (debug) {
	cout << "YAML_BLOCK_END_TOKEN" << endl;
	cout << "path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }
      if (!dry_run) {
	curr_index[curr_index.size() - 1] = -1;
      }
      done = true;
      break;

    case YAML_BLOCK_MAPPING_START_TOKEN:
      if (debug) {
	cout << "YAML_BLOCK_MAPPING_START_TOKEN" << endl;
	cout << " path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }      
      handle_YAML_BLOCK_MAPPING_START_TOKEN(parser);
      break;

    default:
      if (debug) {
	cout << "handle_YAML_BLOCK_SEQUENCE_START_TOKEN: unexpected token " << token.type << endl;
      }
      if (!dry_run) {
	ostringstream m;
	m << "handle_YAML_BLOCK_SEQUENCE_START_TOKEN: unexpected token " << token.type << endl
	  << "path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
					     throw runtime_error(m.str());
      }
      
      break;      
    }

    yaml_token_delete(&token);
  } while (!done);
}

void YMLConfig::parse(const string& yml)
{
  YMLConfigPP pp(pp_pathes);
  auto yml_fn = pp.find_yml_file(yml);
  if (yml_fn.second == false) {
    ostringstream m; m << "YMLConfig::parse: can't find yml " << yml;
    throw runtime_error(m.str());
  }

  this->parse_file__(yml_fn.first, this->pp_pathes);
}

void YMLConfig::parse_file__(const string& yml_fn__,
			     const vector<string>& pp_pathes__)
{
  yaml_parser_t parser;
  /* Initialize parser */
  if (!yaml_parser_initialize(&parser)) {
    ostringstream m; m << "YMLConfig::parse: failed to initialize parser, file was " << yml_fn__;
    throw runtime_error(m.str());
  }

#if 0
  /* Set input file */
  yaml_parser_set_input_file(&parser, fh);
#else
  YMLConfigPP pp(pp_pathes__);
  pp.run_pp(yml_fn__);
  string pp_content; pp.get_pp_content(&pp_content);
  
  yaml_parser_set_input_string(&parser, (const unsigned char*)pp_content.c_str(), pp_content.size());
#endif

  yaml_token_t  token;
  bool done = false;
  do {
    yaml_parser_scan(&parser, &token);
    switch(token.type)
    {
    case YAML_STREAM_START_TOKEN:
      if (debug) cout << "YAML_STREAM_START_TOKEN" << endl;
      break;
    case YAML_STREAM_END_TOKEN:
      if (debug) cout << "YAML_STREAM_END_TOKEN" << endl;
      done = true;
      break;
    case YAML_BLOCK_MAPPING_START_TOKEN:
      if (debug) {
	cout << "YAML_BLOCK_MAPPING_START_TOKEN" << endl;
	cout << " path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }
      handle_YAML_BLOCK_MAPPING_START_TOKEN(&parser);
      break;
    case YAML_BLOCK_SEQUENCE_START_TOKEN:      
      if (debug) {
	cout << "YAML_BLOCK_SEQUENCE_START_TOKEN" << endl;
	cout << " path: " << YMLConfigPath(curr_path, curr_index).to_string() << endl;
      }
      handle_YAML_BLOCK_SEQUENCE_START_TOKEN(&parser);
      break;
    case YAML_ALIAS_TOKEN: 
      if (debug) {
	cout << "YAML_ALIAS_TOKEN" << endl;
	cout << " alias: " << token.data.alias.value << endl;
      }
      if (!dry_run) {
	ostringstream m; m << "YMLConfig::parse: aliases is not supported" << endl;
	throw runtime_error(m.str());
      }
      break;
    case YAML_ANCHOR_TOKEN:
      if (debug) {
	cout << "YAML_ANCHOR_TOCKEN" << endl;
	cout << " anchor: " << token.data.anchor.value << endl;
      }
      if (!dry_run) {
	ostringstream m; m << "YMLConfig::parse: anchors is not supported" << endl;
	throw runtime_error(m.str());
      }
      break;
    case YAML_NO_TOKEN:
      if (debug) {
	cout << "YAML_NO_TOKEN" << endl;
      }
      if (!dry_run) {
	ostringstream m; m << "YMLConfig::parse: YAML_NO_TOKEN -- error in file " << yml_fn__;
	throw runtime_error(m.str());
      }
      break;
    default:
      if (debug) {
	cout << "default: got token of type " << token.type << endl;
      }
      if (!dry_run) {
	cout << "Got token of type " << token.type << endl;
	ostringstream m; m << "YMLConfig::parse: got token of type " << token.type << ", file was " << yml_fn__;
	throw runtime_error(m.str());
      }
      break;
    }
    
    yaml_token_delete(&token);
  } while(!done);

  for (const auto& [key, value]: this->values) {
    this->values_map[key] = value;
  }
  
  /* Cleanup */
  yaml_parser_delete(&parser);

#if 0
  fclose(fh);
#endif
}

void YMLConfig::dump() const
{
  if (this->values.size() == 0) {
    cout << "-- empty YMLConfig --, stem " << stem.to_string() << endl;
    return;
  }

  cout << "stem: " << stem.to_string() << endl;
  for (const auto& [key, value]: this->values) {
    cout << key.to_string() << " --> " << value << endl;
  }
}

YMLConfig YMLConfig::get_config_list_element(int elem_index) const
{
  YMLConfig ret;
  for (const auto& [key, value]: this->values) {
    //cout << "get_config_list_element: " << key.to_string() << endl;
    if (key.path[0].second == elem_index) {
      YMLConfigPath new_path;
      copy(key.path.begin() + 1, key.path.end(),
	   back_inserter(new_path.path));
      ret.values.push_back(make_pair(new_path, value));
      ret.values_map[new_path] = value;
    }
  }
  return ret;
}

YMLConfig YMLConfig::get_config(const char* path_) const
{
  YMLConfigPath path; path.from_string(path_);
  //cout << "path: " << path.to_string() << endl;
  
  YMLConfig ret(path_);
  ret.stem = path;
  for (const auto& [key, value]: this->values) {
    //cout << "key: " << key.to_string() << endl;
    if (path.is_subpath_of(key)) {
      YMLConfigPath new_path;
      if (path.path[1].second == -1 && key.path[1].second >= 0) {
	ret.config_list_nof_elems = key.path[1].second;
	new_path.path[0].second = key.path[1].second;
      }
      copy(key.path.begin() + path.path.size(), key.path.end(),
	   back_inserter(new_path.path));
      ret.values.push_back(make_pair(new_path, value));
      ret.values_map[new_path] = value;
    }
  }

  return ret;
}

string YMLConfig::get(const char* path) const
{
  YMLConfigPath p; p.from_string(path);
  auto it = this->values_map.find(p);
  if (it == this->values_map.end()) {
    ostringstream m;
    m << "YMLConfig::get: no such path: " << path << " at node "
      << stem.to_string() << endl;
    throw runtime_error(m.str());
  }
  return (*it).second;
}
