#include <yajl/yajl_parse.h>
#include <kvan/vector-json-io.h>

struct parser_ctx {
  vector<pair<string, string>>* ret;
  vector<pair<string, int>> curr_path;

  parser_ctx(vector<pair<string, string>>* ret) {
    this->ret = ret;
    curr_path.push_back(make_pair("", -1));
  }
  string get_curr_path();
  
  int process_null();
  int process_boolean(int boolean);
  int process_number(const string&);
  int process_string(const string&);
  int process_map_key(const string&);
  int process_start_map();
  int process_end_map();
  int process_start_array();
  int process_end_array();
};

string parser_ctx::get_curr_path()
{
  ostringstream out;
  for (size_t i = 0; i < curr_path.size(); i++) {
    out << curr_path[i].first;
    if (curr_path[i].second >= 0) {
      out << "[" << curr_path[i].second << "]";
    }
    if (i + 1 < curr_path.size()) {
      out << ".";
    }
  }
  return out.str();
}

int parser_ctx::process_null()
{
  cout << __func__ << endl;
  return 1;
}

int parser_ctx::process_boolean(int b)
{
  //cerr << "parser_ctx::process_boolean: " << b << endl;
  cout << __func__ << endl;
  cout << get_curr_path() << ": " << b << endl;
  if (curr_path.back().second >= 0) { curr_path.back().second++; }
  return 1;
}

int parser_ctx::process_number(const string& n)
{
  cout << __func__ << endl;
  cout << get_curr_path() << ": " << n << endl;
  if (curr_path.back().second >= 0) { curr_path.back().second++; }
  return 1;
}

int parser_ctx::process_string(const string& s)
{
  cout << __func__ << endl;
  cout << get_curr_path() << ": " << s << endl;
  if (curr_path.back().second >= 0) { curr_path.back().second++; }
  return 1;
}

int parser_ctx::process_map_key(const string& k)
{
  cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.back().first = k;
  return 1;
}
  
int parser_ctx::process_start_map()
{
  cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.push_back(make_pair("", -1));
  return 1;
}

int parser_ctx::process_end_map()
{
  cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.pop_back();
  if (curr_path.back().second >= 0) {
    curr_path.back().second++;
  }
  return 1;
}

int parser_ctx::process_start_array()
{
  cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.back().second = 0;
  return 1;
} 

int parser_ctx::process_end_array()
{
  cerr << __func__ << " " << get_curr_path() << endl;
  curr_path.back().second = -1;
  return 1;
}

static int reformat_null(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_null();
}

static int reformat_boolean(void * ctx, int boolean)
{
  auto t = (parser_ctx*)ctx;
  return t->process_boolean(boolean);
}

static int reformat_number(void * ctx, const char * s, size_t l)
{
  auto t = (parser_ctx*)ctx;
  string v(s, l);
  return t->process_number(v);
}

static int reformat_string(void * ctx, const unsigned char * stringVal, size_t stringLen)
{
  auto t = (parser_ctx*)ctx;
  string v((char*)stringVal, stringLen);
  return t->process_string(v);
}

static int reformat_map_key(void * ctx, const unsigned char * stringVal, size_t stringLen)
{
  auto t = (parser_ctx*)ctx;
  string map_key((char*)stringVal, stringLen);
  return t->process_map_key(map_key);
}

static int reformat_start_map(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_start_map();
}

static int reformat_end_map(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_end_map();
}

static int reformat_start_array(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_start_array();
}

static int reformat_end_array(void * ctx)
{
  auto t = (parser_ctx*)ctx;
  return t->process_end_array();
}

static yajl_callbacks cbs = {
  reformat_null,
  reformat_boolean,
  NULL,
  NULL,
  reformat_number,
  reformat_string,
  reformat_start_map,
  reformat_map_key,
  reformat_end_map,
  reformat_start_array,
  reformat_end_array
};

vector<pair<string, string>> from_json(const string& json_s)
{
  vector<pair<string, string>> ret;
  parser_ctx ctx(&ret);
  
  auto hand = yajl_alloc(&cbs, NULL, &ctx);
  yajl_config(hand, yajl_allow_comments, 1);
  
  yajl_status stat = yajl_parse(hand, (const unsigned char*)json_s.c_str(), json_s.size());
  if (stat != yajl_status_ok) {
    stat = yajl_complete_parse(hand);
    if (stat != yajl_status_ok) {
      throw runtime_error("from_json: yajl failed");
    }
  }

  yajl_free(hand);
  return ret;
}
  
    
