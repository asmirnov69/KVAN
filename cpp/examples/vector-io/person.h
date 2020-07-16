// -*- c++ -*-
#ifndef __KVAN_PERSON_HH__
#define __KVAN_PERSON_HH__

enum class Salutation { MR, MRS, MS };
template <> inline string get_enum_value_string<Salutation>(Salutation s) {
  string ret;
  switch (s) {
  case Salutation::MR: ret = "Mr."; break;
  case Salutation::MRS: ret = "Mrs."; break;
  case Salutation::MS: ret = "Ms."; break;
  }
  return ret;
}

template <> inline
void set_enum_value<Salutation>(Salutation* o, const string& new_v)
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
template <> inline string get_enum_value_string<State>(State s) {
  string ret;
  switch (s) {
  case State::NY: ret = "NY"; break;
  case State::CT: ret = "CT"; break;
  case State::MA: ret = "MA"; break;
  case State::RI: ret = "RI"; break;
  }
  return ret;
}

template <> inline void set_enum_value<State>(State* o, const string& new_v)
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
inline StructDescriptor get_struct_descriptor<FullName>()
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
inline StructDescriptor get_struct_descriptor<Address>()
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

struct Parents
{
  vector<Person> parents;
};

template <>
inline StructDescriptor get_struct_descriptor<Parents>()
{
  StructDescriptor sd;
  sd.add_member("parents", &Parents::parents);
  return sd;
}

typedef vector<Person> PersonSeq;

struct Band
{
  string name;
  vector<Person> band_members;
  vector<PersonSeq> band_member_parents;
  vector<double> ws;
};

template <>
inline StructDescriptor get_struct_descriptor<Band>()
{
  StructDescriptor sd;
  sd.add_member("name", &Band::name);
  sd.add_member("band_members", &Band::band_members);
  sd.add_member("band_member_parents", &Band::band_member_parents);
  sd.add_member("ws", &Band::ws);
  return sd;
}

#endif
