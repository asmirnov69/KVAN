#include <kvan/csv-io.h>

void CSVColumnsVisitor::visit_enum(const LOBKey& path, const string& enum_s)
{
  cols.push_back(path);
}
    
void CSVColumnsVisitor::visit_string(const LOBKey& path, const string& s)
{
  cols.push_back(path);
}

void CSVColumnsVisitor::visit_fundamental(const LOBKey& path, const string& v)
{
  cols.push_back(path);
}

void CSVColumnsVisitor::visit_start_map(const LOBKey& path)
{
}
  
void CSVColumnsVisitor::visit_end_map()
{
}

void CSVColumnsVisitor::visit_delimiter()
{
}

void CSVColumnsVisitor::visit_start_array()
{
}

void CSVColumnsVisitor::visit_end_array()
{
}

// ...

void CSVVisitor::visit_enum(const LOBKey& path, const string& enum_s)
{
  out << enum_s;
}

void CSVVisitor::visit_string(const LOBKey& path, const string& s)
{
  out << s;
}

void CSVVisitor::visit_fundamental(const LOBKey& path, const string& v)
{
  out << v;
}

void CSVVisitor::visit_start_map(const LOBKey& path)
{
}
void CSVVisitor::visit_end_map()
{
}

void CSVVisitor::visit_delimiter()
{
  out << ",";
}
void CSVVisitor::visit_start_array()
{
  throw runtime_error("not implemented");
}

void CSVVisitor::visit_end_array()
{
  throw runtime_error("not implemented");
}

// ....

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

void to_csv_line(ostream& out, const vector<LOBKey>& cols, const LOB& l)
{
  for (auto it = cols.begin(); it != cols.end(); ++it) {
    out << l.get(*it);
    if (next(it) != cols.end()) {
      out << ",";
    }
  }
}

void to_csv(ostream& out, const LOBVector& v)
{
  auto lob_keys = v.keys;
  vector<string> columns;
  for (auto& el: lob_keys) {
    columns.push_back(string_join(el, '.'));
  }
  out << string_join(columns, ',') << endl;

  for (size_t i = 0; i < v.lobs.size(); i++) {
    to_csv_line(out, lob_keys, v.lobs[i]);
    out << endl;
  }
}

void from_csv(LOBVector* v, istream& in)
{
  string line;
  getline(in, line);
  vector<string> columns = string_split(line, ',');
  vector<LOBKey> lob_keys;
  for (auto& col: columns) {
    lob_keys.push_back(string_split(col, '.'));
  }

  v->init(lob_keys);
  int line_no = 1;

  while (getline(in, line)) {
    vector<string> values = parse_csv_line(line);
    //cout << "csv line: " << string_join(values, ';') << endl;
    if (values.size() != columns.size()) {
      ostringstream m; m << "columns/values mismatch at line " << line_no;
      throw runtime_error(m.str());
    }
    LOB new_v;
    for (size_t i = 0; i < lob_keys.size(); i++) {
      auto lob_key = lob_keys[i];
      auto new_value = values[i];
      new_v.set(lob_key, new_value);
    }
    v->push_back(new_v);
    line_no++;
  }
}
