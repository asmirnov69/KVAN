#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/vector-csv-io.h>
#include <kvan/vector-fjson-io.h>
#include <kvan/vector-json-io.h>

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
    
    auto [cols, json_df] = to_fjson(persons);
    copy(cols.begin(), cols.end(), ostream_iterator<string>(cerr, ","));
    cerr << endl;
    cout << json_df << endl;
    
    return true;
  });

ADD_ACTION("write_json[]", [](const Fuargs::args&) {
    vector<Person> persons;
    persons.push_back(p1);
    persons.push_back(p2);
    persons.push_back(p2);
    
    Band b;
    b.name = "ura";
    b.band_members = persons;
    b.ws.push_back(1.0);
    b.ws.push_back(2.0);
    
    //Parents pp;
    //pp.parents.push_back(persons[0]);
    //PersonSeq psq;
    //psq.push_back(persons[0]);
    //b.band_member_parents.push_back(psq);

    ostringstream json_s;
    //to_json(json_s, persons);
    to_json(json_s, b);
    cout << json_s.str() << endl;
    
    return true;
  });

ADD_ACTION("write_csv[]", [](const Fuargs::args&) {
    vector<Person> persons;
    persons.push_back(p1);
    persons.push_back(p2);
    persons.push_back(p2);

    ostringstream csv_s; to_csv(csv_s, persons);
    cout << csv_s.str();
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
    cout << to_fjson(persons).second << endl;

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
    cout << to_fjson(tickers).second << endl;

    return true;
  });

int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv);
}

