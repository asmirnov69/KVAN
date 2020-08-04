// -*- c++ -*-
#ifndef __KVAN_PERSON_HH__
#define __KVAN_PERSON_HH__

#include <sstream>
using namespace std;

#include <kvan/struct-descriptor.h>

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
  static const StructDescriptor sd = {
    make_member_descriptor("first_name", &FullName::first_name),
    make_member_descriptor("last_name", &FullName::last_name),
    make_member_descriptor("salutation", &FullName::salutation)
  };
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
  static const StructDescriptor sd = {
    make_member_descriptor("line1", &Address::line1),
    make_member_descriptor("line2", &Address::line2),
    make_member_descriptor("state", &Address::state),
    make_member_descriptor("zip", &Address::zip)
  };
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
  static const StructDescriptor sd = {
    make_member_descriptor("email", &Contact::email),
    make_member_descriptor("phone", &Contact::phone)
  };
  return sd;
}

struct Person
{
  vector<string> sss;
  FullName name;
  Address address;
  Contact c, backup_c;
  int age;
  double height;
};

template <>
inline StructDescriptor get_struct_descriptor<Person>()
{
  static const StructDescriptor sd = {
    make_member_descriptor("sss", &Person::sss),
    make_member_descriptor("name", &Person::name),
    make_member_descriptor("address", &Person::address),
    make_member_descriptor("c", &Person::c),
    make_member_descriptor("backup_c", &Person::backup_c),
    make_member_descriptor("age", &Person::age),
    make_member_descriptor("height", &Person::height)
  };
  return sd;
}

struct Parents
{
  vector<Person> parents;
};

template <>
inline StructDescriptor get_struct_descriptor<Parents>()
{
  static const StructDescriptor sd = {
    make_member_descriptor("parents", &Parents::parents)
  };
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
  static const StructDescriptor sd = {
    make_member_descriptor("name", &Band::name),
    make_member_descriptor("band_members", &Band::band_members),
    make_member_descriptor("band_member_parents", &Band::band_member_parents),
    make_member_descriptor("ws", &Band::ws)
  };
  return sd;
}

#endif
