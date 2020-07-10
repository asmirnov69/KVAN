#include <stdexcept>
#include <sstream>
#include <iostream>
using namespace std;

#include <kvan/string-utils.h>

vector<string> string_split(const string& s, char del)  
{
  vector<string> ret;
  string p_s;
  istringstream in(s);
  while (getline(in, p_s, del)) {
    ret.push_back(p_s);
  }
  return ret;
}

string string_join(const vector<string>& vs, char del)
{
  ostringstream out;
  for (size_t i = 0; i < vs.size(); i++) {
    out << vs[i];
    if (i + 1 < vs.size()) {
      out << del;
    }
  }
  return out.str();
}
			    
vector<string> parse_csv_line(const string& line)
{
  enum class parser_state {
    in_cell, in_cell_expect_second_quote,
    out_of_cell
  };

  vector<string> ret;
  parser_state state{parser_state::out_of_cell};

  string cell;
  for (auto c: line) {
    //cout << "c: " << c << endl;
    if (c == ',') {
      switch (state) {
      case parser_state::in_cell:
	ret.push_back(cell); cell = "";
	state = parser_state::out_of_cell;
	break;
      case parser_state::in_cell_expect_second_quote:
	ret.push_back(cell); cell = "";
	state = parser_state::out_of_cell;
	break;      
      case parser_state::out_of_cell:
	ret.push_back(""); cell = "";
	state = parser_state::out_of_cell;
	break;
      }
    } else if (c == '"') {
      switch (state) {
      case parser_state::in_cell:
	state = parser_state::in_cell_expect_second_quote;
	break;
      case parser_state::in_cell_expect_second_quote: 
	cell += '"';
	state = parser_state::in_cell;
	break;
      case parser_state::out_of_cell:
	state = parser_state::in_cell;
	break;
      }
    } else {
      switch (state) {
      case parser_state::in_cell:
	cell += c;
	state = parser_state::in_cell;
	break;
      case parser_state::in_cell_expect_second_quote:
	{
	  ostringstream m;
	  m << "malformed csv line: " << line
	    << ", cell: " << cell
	    << ", c: " << c;
	  throw runtime_error(m.str());
	}
	break;
      case parser_state::out_of_cell:
	cell += c;
	state = parser_state::in_cell;
	break;	
      }
    }
  }
  
  ret.push_back(cell);
  
  return ret;
}
