// -*- c++ -*-
#ifndef __RQX_STRUCT_DESCRIPTOR__HH__
#define __RQX_STRUCT_DESCRIPTOR__HH__

#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <type_traits>
#include <any>
using namespace std;

struct StructDescriptor;
template <class T> string get_enum_value_string(T);
template <class T> StructDescriptor get_struct_descriptor();
typedef vector<pair<list<string>, string>> JKV; // json key -> value

class MemberDescriptor
{
public:
  string member_name;
  MemberDescriptor(const char* member_name) {
    this->member_name = member_name;
  }

  virtual void get_columns__(vector<list<string>>* out, list<string>* path) = 0;
  virtual void get_value__(const any&, JKV*, list<string>*) = 0;
};

// from https://stackoverflow.com/a/57812868/1181482
template<typename T>
struct is_string
        : public std::disjunction<
                std::is_same<char *, typename std::decay<T>::type>,
                std::is_same<const char *, typename std::decay<T>::type>,
                std::is_same<std::string, typename std::decay<T>::type>
        > {
};

template <class MT, class T>
class MemberDescriptorT : public MemberDescriptor
{
private:
  MT T::*mptr;

public:
  MemberDescriptorT(const char* member_name, MT T::*mptr) :
    MemberDescriptor(member_name)
  {
    this->mptr = mptr;
  }

  void get_columns__(vector<list<string>>* out, list<string>* path) override
  {
    path->push_back(member_name);
    if constexpr(is_enum<MT>::value) {
      out->push_back(*path);
    } else if constexpr(is_string<MT>::value) {
      out->push_back(*path);
    } else if constexpr(is_integral<MT>::value) {
      out->push_back(*path);
    } else if constexpr(is_floating_point<MT>::value) {
      out->push_back(*path);       
    } else if constexpr(is_function<decltype(get_struct_descriptor<MT>)>::value) {
      auto sd = get_struct_descriptor<MT>();
      sd.get_columns__(out, path);
    } else {
      throw runtime_error(__func__);
    }
    path->pop_back();
  }

  void get_value__(const any& o, JKV* out, list<string>* path) override
  {
    try {
      const T& obj = any_cast<T>(o);
      const MT& member_v = obj.*mptr;

      path->push_back(member_name);
      if constexpr(is_enum<MT>::value) {
	  out->push_back(make_pair(*path, "\"" + get_enum_value_string<MT>(member_v) + "\""));
      } else if constexpr(is_string<MT>::value) {
	  out->push_back(make_pair(*path, "\"" + member_v + "\""));
      } else if constexpr(is_integral<MT>::value) {
	  out->push_back(make_pair(*path, to_string(member_v)));
      } else if constexpr(is_floating_point<MT>::value) {
	  out->push_back(make_pair(*path, to_string(member_v)));
      } else if constexpr(is_function<decltype(get_struct_descriptor<MT>)>::value) {
	auto m_sd = get_struct_descriptor<MT>();
	m_sd.get_value__(member_v, out, path);
      } else {
	throw runtime_error(__func__);
      }
      path->pop_back();
    } catch (const bad_any_cast& ex) {
	throw runtime_error(ex.what());      
      }
  }
};

class StructDescriptor
{
public:
  vector<string> member_names;
  vector<unique_ptr<MemberDescriptor>> member_descriptors;
  
  template <class MT, class T>
  void add_member(const char* member_name, MT T::*mptr)
  {
    member_names.push_back(member_name);
    member_descriptors.emplace_back(make_unique<MemberDescriptorT<MT, T>>(member_name, mptr));
  }

  void get_columns__(vector<list<string>>* out, list<string>* path)
  {
    for (auto& m_descr: member_descriptors) {
      m_descr->get_columns__(out, path);
    }
  }

  void get_columns(vector<string>* out)
  {
    list<string> path;
    vector<list<string>> pathes;
    get_columns__(&pathes, &path);
    for (auto& p: pathes) {
      string cp;
      for (auto it = p.begin(); it != p.end(); ++it) {
	cp += *it;
	if (next(it) != p.end()) {
	  cp += ".";
	}
      }
      out->push_back(cp);
    }
  }
  
  void get_value__(const any& o, JKV* out, list<string>* path)
  {
    for (auto& m_descr: member_descriptors) {
      m_descr->get_value__(o, out, path);
    }
  }

  void get_value(const any& o, JKV* out) {
    list<string> path;
    get_value__(o, out, &path);
  }
};

#endif
