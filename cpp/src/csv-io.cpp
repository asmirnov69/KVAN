#include <kvan/csv-io.h>

string to_csv_string(const vector<ValuePathValue>& j)
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
