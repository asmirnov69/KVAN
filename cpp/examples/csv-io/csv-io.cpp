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

    vector<LOB> person_lobs;
    for (auto& p: persons) {
      LOB l; to_LOB(&l, p);
      person_lobs.push_back(l);
    }

    to_fjson(cout, person_lobs);
    cerr << endl;
    
    return true;
  });

ADD_ACTION("write_csv[]", [](const Fuargs::args&) {
    vector<Person> persons;
    persons.push_back(p1);
    persons.push_back(p2);
    persons.push_back(p2);
    
    LOBVector person_lobs;
    person_lobs.init(get_struct_descriptor<Person>().get_value_pathes());
    for (auto& p: persons) {
      LOB l; to_LOB(&l, p);
      person_lobs.push_back(l);
    }
    
    to_csv(cout, person_lobs);

    return true;
  });

ADD_ACTION("read_csv[fn]", [](const Fuargs::args& args) {
    string fn = args.get("fn");
    ifstream in(fn);
    if (!in) {
      cerr << "can't open file " << fn << endl;
      return false;
    }

    LOBVector person_lobs;
    from_csv(&person_lobs, in);

    vector<Person> persons;
    for (auto& l: person_lobs) {
      Person p; from_LOB(&p, l);
      persons.emplace_back(p);
    }
    
    cout << "persons size: " << persons.size() << endl;
    //cout << to_fjson(persons).second << endl;

    return true;
  });


ADD_ACTION("read_tickers_csv[fn]", [](const Fuargs::args& args) {
    string fn = args.get("fn");
    ifstream in(fn);
    if (!in) {
      cerr << "can't open file " << fn << endl;
      return false;
    }

    LOBVector ticker_lobs;
    from_csv(&ticker_lobs, in);    
    vector<Ticker> tickers;
    for (auto& l: ticker_lobs) {
      Ticker t; from_LOB(&t, l);
      tickers.emplace_back(t);
    }
    cout << "tickers size: " << tickers.size() << endl;

    {
      vector<LOB> lv;
      for (auto& t: tickers) {
	LOB l; to_LOB(&l, t); lv.push_back(l);
      }
      ostringstream out;
      to_fjson(out, lv);
      cout << out.str() << endl;
    }
    
    return true;
  });

int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv);
}
