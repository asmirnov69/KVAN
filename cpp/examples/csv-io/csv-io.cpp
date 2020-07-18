#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/csv-io.h>
#include <kvan/fjson-io.h>

#include "person.h"
#include "ticker.h"

Person p1{.name = FullName{.first_name = "John", .last_name = "Smith", .salutation = Salutation::MR},
    .address = Address{.line1 = "1 Main St", .line2 = "", .city = "New York", .state = State::NY, .zip = "10001"},
       .c = Contact{.phone="123", .email="a@b.c"}, .backup_c = Contact(),
						      .age = 42, .height = 1.72};
Person p2{.name = FullName{.first_name = "Jim", .last_name = "Morrison", .salutation = Salutation::MR},
    .address = Address{.line1 = "1 Main St", .line2 = "", .city = "Boston", .state = State::MA, .zip = "02142"},
       .c = Contact(), .backup_c = Contact(),
	  .age = 27, .height = 1.8};

ADD_ACTION("write_fjson[]", [](const Fuargs::args&) {
    vector<Person> persons;
    persons.push_back(p1);
    persons.push_back(p2);
    persons.push_back(p2);

    to_fjson<Person>(cout, persons);
    cout << endl;
    
    return true;
  });

ADD_ACTION("write_csv[]", [](const Fuargs::args&) {
    vector<Person> persons;
    persons.push_back(p1);
    persons.push_back(p2);
    persons.push_back(p2);

    auto sd = get_struct_descriptor<Person>();
    CSVColumnsVisitor cols_v;
    LOBKey k;
    sd.visit_members(&cols_v, &k, p1);

    cout << cols_v.get_header() << endl;
    for (auto& p: persons) {
      CSVVisitor csv_v(cout, cols_v.cols);
      LOBKey k;
      sd.visit_members(&csv_v, &k, p);
      cout << endl;
    }
    
    return true;
  });

ADD_ACTION("read_csv[fn]", [](const Fuargs::args& args) {
    string fn = args.get("fn");
    ifstream in(fn);
    if (!in) {
      cerr << "can't open file " << fn << endl;
      return false;
    }

    vector<Person> persons;
    from_csv(&persons, in);
    
    cout << "persons size: " << persons.size() << endl;
    to_fjson(cout, persons); cout << endl;

    return true;
  });

ADD_ACTION("read_tickers_csv[fn]", [](const Fuargs::args& args) {
    string fn = args.get("fn");
    ifstream in(fn);
    if (!in) {
      cerr << "can't open file " << fn << endl;
      return false;
    }

    vector<Ticker> tickers;
    from_csv(&tickers, in);
    
    cout << "tickers size: " << tickers.size() << endl;
    to_fjson(cout, tickers); cout << endl;

    return true;
  });

int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv);
}
