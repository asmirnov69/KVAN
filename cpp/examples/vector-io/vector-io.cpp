#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/vector-io.h>

template <class T> string get_enum_value_string(T);
template <class T> void set_enum_value(T*, const string& new_value);

enum class Salutation { MR, MRS, MS };
template <> string get_enum_value_string<Salutation>(Salutation s) {
  string ret;
  switch (s) {
  case Salutation::MR: ret = "Mr."; break;
  case Salutation::MRS: ret = "Mrs."; break;
  case Salutation::MS: ret = "Ms."; break;
  }
  return ret;
}

template <> void set_enum_value<Salutation>(Salutation* o, const string& new_v)
{
  if (new_v == "Mr.") {
    *o = Salutation::MR;
  } else if (new_v == "Mrs.") {
    *o = Salutation::MRS;
  } else if (new_v == "Ms.") {
    *o = Salutation::MS;
  } else {
    ostringstream m; m << "unknown salutaion " << new_v;
    throw runtime_error(m.str());
  }
}

enum class State { MA, NY, CT, RI };
template <> string get_enum_value_string<State>(State s) {
  string ret;
  switch (s) {
  case State::NY: ret = "NY"; break;
  case State::CT: ret = "CT"; break;
  case State::MA: ret = "MA"; break;
  case State::RI: ret = "RI"; break;
  }
  return ret;
}
template <> void set_enum_value<State>(State* o, const string& new_v)
{
  if (new_v == "NY") {
    *o = State::NY;
  } else if (new_v == "CT") {
    *o = State::CT;
  } else if (new_v == "MA") {
    *o = State::MA;
  } else if (new_v == "RI") {
    *o = State::RI;
  } else {
    ostringstream m; m << "unknown state: " << new_v;
    throw runtime_error(m.str());
  }
}

struct FullName
{
  string first_name;
  string last_name;
  Salutation salutation;
};
template <>
StructDescriptor get_struct_descriptor<FullName>()
{
  StructDescriptor sd;
  sd.add_member("first_name", &FullName::first_name);
  sd.add_member("last_name", &FullName::last_name);
  sd.add_member("salutation", &FullName::salutation);
  return sd;
}

struct Address
{
  string line1, line2;
  string city;
  State state;
  string zip;
};
template <>
StructDescriptor get_struct_descriptor<Address>()
{
  StructDescriptor sd;
  sd.add_member("line1", &Address::line1);
  sd.add_member("line2", &Address::line2);
  sd.add_member("state", &Address::state);
  sd.add_member("zip", &Address::zip);
  return sd;
}

struct Contact
{
  string phone;
  string email;
};

template <>
inline StructDescriptor get_struct_descriptor<Contact>()
{
  StructDescriptor sd;
  sd.add_member("email", &Contact::email);
  sd.add_member("phone", &Contact::phone);
  return sd;
}

struct Person
{
  FullName name;
  Address address;
  Contact c, backup_c;
  int age;
  double height;
};

template <>
inline StructDescriptor get_struct_descriptor<Person>()
{
  StructDescriptor sd;
  sd.add_member("name", &Person::name);
  sd.add_member("address", &Person::address);
  sd.add_member("c", &Person::c);
  sd.add_member("backup_c", &Person::backup_c);
  sd.add_member("age", &Person::age);
  sd.add_member("height", &Person::height);
  return sd;
}

Person p1{.name = FullName{.first_name = "John", .last_name = "Smith", .salutation = Salutation::MR},
    .address = Address{.line1 = "1 Main St", .line2 = "", .city = "New York", .state = State::NY, .zip = "10001"},
       .c = Contact{.phone="123", .email="a@b.c"}, .backup_c = Contact(),
						      .age = 42, .height = 1.72};
Person p2{.name = FullName{.first_name = "Jim", .last_name = "Morrison", .salutation = Salutation::MR},
    .address = Address{.line1 = "1 Main St", .line2 = "", .city = "Boston", .state = State::MA, .zip = "02142"},
       .c = Contact(), .backup_c = Contact(),
	  .age = 27, .height = 1.8};


ADD_ACTION("write_json[]", [](const Fuargs::args&) {
    vector<Person> persons;
    persons.push_back(p1);
    persons.push_back(p2);
    persons.push_back(p2);
    
    auto [cols, json_df] = to_json_dataframe(persons);
    copy(cols.begin(), cols.end(), ostream_iterator<string>(cerr, ","));
    cerr << endl;
    cout << json_df << endl;
    
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
    cout << to_json_dataframe(persons).second << endl;

    return true;
  });

int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv);
}

