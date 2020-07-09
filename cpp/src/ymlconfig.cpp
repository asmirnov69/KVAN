#include <sstream>
#include <stdexcept>
#include <iostream>
using namespace std;

#include <kvan/ymlconfig.h>
#include <yaml.h>
extern "C" {
#include <mcpp_lib.h>
}

bool operator<(const YMLConfigPath& l, const YMLConfigPath& r)
{
  return l.to_string() < r.to_string();
}

YMLConfigPath::YMLConfigPath(const vector<string>& curr_path,
			     const vector<int>& curr_index)
{
  if (curr_path.size() != curr_index.size()) {
    throw runtime_error("YMLConfigPath::YMLConfigPath: path and index size mismatch");
  }
  for (size_t i = 0; i < curr_path.size(); i++) {
    this->path.push_back(make_pair(curr_path[i], curr_index[i]));
  }
}

YMLConfigPath::YMLConfigPath()
{
}

bool YMLConfigPath::is_subpath_of(const YMLConfigPath& p)
{
  bool ret = false;
  if (this->path.size() <= p.path.size()) {
    ret = true;
    for (size_t i = 0; i < this->path.size(); i++) {
      if (!(p.path[i].first == this->path[i].first
	    && p.path[i].second == this->path[i].second)) {
	ret = false;
	break;
      }
    }
  }
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

YMLConfig::YMLConfig(const string& stem,
		     bool enable_dollar_var_expansion, bool debug, bool dry_run)
{
  this->stem = stem;
  this->enable_dollar_var_expansion = enable_dollar_var_expansion;
  this->debug = debug;
  this->dry_run = dry_run;
}

// ENV{env-var-name}
static string evaluate_dollar_var_expr(const string& dv_expr)
{
  string ret;
  
  size_t prefix_end_index = dv_expr.find("{");
  string prefix = dv_expr.substr(0, prefix_end_index - 0);
  string arg = dv_expr.substr(prefix_end_index+1,
			      dv_expr.size() - 1 - (prefix_end_index+1));
  if (prefix == "ENV") {
    const char* v = getenv(arg.c_str());
    if (v) {
      ret = v;
    } else {
      ostringstream m; m << "can't find env var " << arg;
      throw runtime_error(m.str());
    }
  } else {
    ostringstream m;
    if (prefix.size() > 0) {
      m << "unknown dollar var prefix: " << prefix
	<< ", should be $ENV{var-name}";
    } else {
      m << "prefix was empty in expr "
	<< dv_expr << ", should be $ENV{var-name}";
    }
    throw runtime_error(m.str());
  }

  return ret;
}

static void do_dollar_var_expansion(string* v)
{
  string new_v = *v;
  while (true) {
    size_t start_dv_index = 0;
    size_t end_dv_index = -1;

    start_dv_index = new_v.find("$", end_dv_index + 1);
    if (start_dv_index == string::npos) {
      break;
    }
    end_dv_index = new_v.find("}", start_dv_index + 1);
    if (end_dv_index == string::npos) {
      break;
    }
    string dollar_var_expr = new_v.substr(start_dv_index + 1,
					  end_dv_index + 1 - (start_dv_index + 1));
    string dollar_var_value = evaluate_dollar_var_expr(dollar_var_expr);
    new_v.replace(start_dv_index, end_dv_index + 1 - start_dv_index,
		  dollar_var_value);
  }

  *v = new_v;
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
	    do_dollar_var_expansion(&v);
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

// see also cpp/src/Slice/Preprocessor.cpp in https://github.com/zeroc-ice/ice
bool
YMLConfig::run_preprocessor(const char* yml_fn,
			    const vector<string>& pp_pathes,
			    string* out, string* out_err)
{
  //
  // Build arguments list.
  //
  vector<string> args;
  args.push_back("-e");
  args.push_back("en_us.utf8");
  for (auto& path: pp_pathes) {
    args.push_back("-I" + path);
  }
  args.push_back(yml_fn);
  
  const char** argv = new const char*[args.size() + 1];
  argv[0] = "mcpp";
  for(unsigned int i = 0; i < args.size(); ++i) {
    argv[i + 1] = args[i].c_str();
  }
  
  //
  // Call mcpp using memory buffer.
  //
  mcpp_use_mem_buffers(1);
  int status = mcpp_lib_main(static_cast<int>(args.size()) + 1,
			     const_cast<char**>(argv));
  delete[] argv;

  //
  // Display any errors.
  //
  char* err = mcpp_get_mem_buffer(ERR);
  if (err) {
    *out_err = err;
    status = out_err->find("error:") == string::npos ? 0 : 1;
#if 0
    vector<string> messages = filterMcppWarnings(err);
    for(vector<string>::const_iterator i = messages.begin();
	i != messages.end(); ++i) {
      emitRaw(i->c_str());

      //
      // MCPP FIX: mcpp does not always return non-zero exit status when there is an error.
      //
      if(i->find("error:") != string::npos) {
	status = 1;
      }
    }
#endif
  }

  if (status == 0) {
    //
    // Write output
    //
    char* buf = mcpp_get_mem_buffer(OUT);
    *out = buf;
  }

  //
  // Calling this again causes the memory buffers to be freed.
  //
  mcpp_use_mem_buffers(1);

  return status == 0;
}

void YMLConfig::parse(const char* yml_fn, const vector<string>& pp_pathes)
{
#if 0
  FILE *fh = fopen(yml_fn, "r");
  if (fh == NULL) {
    ostringstream m; m << "YMLConfig::parse: failed to open file: " << yml_fn;
    throw runtime_error(m.str());
  }
#endif
  
  yaml_parser_t parser;  
  /* Initialize parser */
  if (!yaml_parser_initialize(&parser)) {
    ostringstream m; m << "YMLConfig::parse: failed to initialize parser, file was " << yml_fn;
    throw runtime_error(m.str());
  }

#if 0
  /* Set input file */
  yaml_parser_set_input_file(&parser, fh);
#else
  string pp_content, errors;
  if (!run_preprocessor(yml_fn, pp_pathes, &pp_content, &errors)) {
    throw runtime_error("preprocessor failed");
  }
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
	ostringstream m; m << "YMLConfig::parse: YAML_NO_TOKEN -- error in file " << yml_fn;
	throw runtime_error(m.str());
      }
      break;
    default:
      if (debug) {
	cout << "default: got token of type " << token.type << endl;
      }
      if (!dry_run) {
	cout << "Got token of type " << token.type << endl;
	ostringstream m; m << "YMLConfig::parse: got token of type " << token.type << ", file was " << yml_fn;
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
    cout << "-- empty YMLConfig --" << endl;
    return;
  }
  
  for (const auto& [key, value]: this->values) {
    cout << key.to_string() << " --> " << value << endl;
  }
}

YMLConfig YMLConfig::get_config(const char* path_) const
{
  YMLConfigPath path; path.from_string(path_);
  YMLConfig ret(path_);
  for (const auto& [key, value]: this->values) {
    if (path.is_subpath_of(key)) {
      YMLConfigPath new_path;
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
    m << "YMLConfig::get: no such path: " << path << " at node " << stem << endl;
    throw runtime_error(m.str());
  }
  return (*it).second;
}
