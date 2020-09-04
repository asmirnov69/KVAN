#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
using namespace std;

#include <kvan/logger.h>
#include <kvan/fuargs.h>
#include <kvan/json-io.h>
#include <kvan/fjson-io.h>

#include "person.h"

Person p1{.sss = vector<string>{},
    .name = FullName{.first_name = "John", .last_name = "Smith", .salutation = Salutation::MR},
    .address = Address{.line1 = "1 Main St", .line2 = "", .city = "New York", .state = State::NY, .zip = "10001"},
       .c = Contact{.phone="123", .email="a@b.c"}, .backup_c = Contact(),
						      .age = 42, .height = 1.72};
Person p2{.sss = vector<string>{},
    .name = FullName{.first_name = "Jim", .last_name = "Morrison", .salutation = Salutation::MR},
    .address = Address{.line1 = "1 Main St", .line2 = "", .city = "Boston", .state = State::MA, .zip = "02142"},
       .c = Contact(), .backup_c = Contact(),
	  .age = 27, .height = 1.8};

struct void_response {
  int response_id;
  json_null_t retval;
};

template <> inline StructDescriptor get_struct_descriptor<void_response>() {
  static const StructDescriptor sd {
    make_member_descriptor("response_id", &void_response::response_id),
      make_member_descriptor("retval", &void_response::retval)
      };
  return sd;
}

ADD_ACTION("test_void[]", [](const Fuargs::args&) {
    void_response vres{.response_id = 111, .retval = json_null_t()};
    cout << "vres: "; // {response_id: 111, retval: null}
    to_json(cout, vres);
    cout << endl;

    string vres2_s = "{\"response_id\": 222, \"retval\": null}";
    void_response vres2;
    from_json(&vres2, vres2_s);
    cout << "vres2.response_id: " << vres2.response_id << endl;
    //cout << "vres2.retval has value: " << vres2.retval.has_value() << endl;
    return 0;
  });

ADD_ACTION("write_json[]", [](const Fuargs::args&) {
    vector<Person> persons;
    persons.push_back(p1);
    persons.push_back(p2);
    persons.push_back(p2);

#if 1
    Band b;
    b.name = "ura";
    b.band_members = persons;
    b.ws.push_back(1.0);
    b.ws.push_back(2.0);

    //Parents pp;
    //pp.parents.push_back(persons[0]);
    PersonSeq psq;
    psq.push_back(persons[0]);
    psq.push_back(persons[1]);
    b.band_member_parents.push_back(psq);
    b.band_member_parents.push_back(psq);

    to_json(cout, b);
#else
    to_json(cout, p1);
#endif
    return true;
  });

ADD_ACTION("read_json[fn]", [](const Fuargs::args& args) {
    string fn = args.get("fn");
    ifstream in(fn);
    if (!in) {
      cerr << "can't open file " << fn << endl;
      return false;
    }

    stringstream json; json << in.rdbuf();

    cout << "json: " << json.str() << endl;    

    auto kvs = from_json(json.str());
    cout << "kvs:" << endl;
    for (auto& el: kvs) {
      cout << to_string(el.first) << ": " << el.second << endl;
    }
    
    return true;
  });

ADD_ACTION("read_json_person[fn]", [](const Fuargs::args& args) {
    //kvan::logger_setup();
    string fn = args.get("fn");
    ifstream in(fn);
    if (!in) {
      cerr << "can't open file " << fn << endl;
      return false;
    }

    stringstream json; json << in.rdbuf();

    cout << "json: " << json.str() << endl;    
    Person p;
    from_json(&p, json.str());

    vector<Person> pp = {p};
    to_fjson(cout, pp);
    cout << endl;

    return true;
  });

ADD_ACTION("read_json_band[fn]", [](const Fuargs::args& args) {
    string fn = args.get("fn");
    ifstream in(fn);
    if (!in) {
      cerr << "can't open file " << fn << endl;
      return false;
    }

    stringstream json; json << in.rdbuf();

    cout << "json: " << json.str() << endl;    
    Band b;
    from_json(&b, json.str());

    to_json(cout, b);
    cout << endl;

    return true;
  });


int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv, false, false);
}
