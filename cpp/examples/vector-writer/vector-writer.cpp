#include <iostream>
#include <string>
#include <vector>
#include <iterator>
using namespace std;

#include <kvan/struct-descriptor.h>

template <class T> string get_enum_value_string(T);

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

int main()
{  
  Person p1{.name = FullName{.first_name = "John", .last_name = "Smith", .salutation = Salutation::MR},
      .address = Address{.line1 = "1 Main St", .line2 = "", .city = "New York", .state = State::NY, .zip = "10001"},
	 .c = Contact{.phone="123", .email="a@b.c"}, .backup_c = Contact(),
	 .age = 42, .height = 1.72};
  Person p2{.name = FullName{.first_name = "Jim", .last_name = "Morrison", .salutation = Salutation::MR},
      .address = Address{.line1 = "1 Main St", .line2 = "", .city = "Boston", .state = State::MA, .zip = "02142"},
	 .c = Contact(), .backup_c = Contact(),
	 .age = 27, .height = 1.8};
  
  vector<Person> persons;
  persons.push_back(p1);
  persons.push_back(p2);
  persons.push_back(p2);

  auto [cols, json_df] = to_json_dataframe(persons);
  copy(cols.begin(), cols.end(), ostream_iterator<string>(cerr, ","));
  cerr << endl;
  cout << json_df << endl; 
}
