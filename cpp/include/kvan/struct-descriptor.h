// -*- c++ -*-
#ifndef __RQX_STRUCT_DESCRIPTOR__HH__
#define __RQX_STRUCT_DESCRIPTOR__HH__

#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <type_traits>
#include <any>
using namespace std;

#include <kvan/string-utils.h>

struct StructDescriptor;
template <class T> string get_enum_value_string(T);
template <class T> void set_enum_value(T*, const string& new_v) {
  throw runtime_error(__func__);
}
template <class T> StructDescriptor get_struct_descriptor();
typedef vector<string> ValuePath;
typedef pair<ValuePath, string> ValuePathValue; // value path -> value

class MemberDescriptor
{
public:
  string member_name;
  MemberDescriptor(const char* member_name) {
    this->member_name = member_name;
  }

  virtual void get_columns__(vector<ValuePath>* out, ValuePath* path) = 0;
  virtual void get_value__(const any&, vector<ValuePathValue>*, ValuePath*) = 0;
  virtual void set_value__(void* o, const string& new_value,
			   const ValuePath& path,
			   int curr_member_index) = 0;
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

  void get_columns__(vector<ValuePath>* out, ValuePath* path) override
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

  void set_value__(void* o, const string& new_value,
		   const vector<string>& path, int curr_member_index) override
  {
    T* obj = reinterpret_cast<T*>(o);
    MT& member_v = obj->*mptr;
    
    if constexpr(is_enum<MT>::value) {
	set_enum_value<MT>(&member_v, new_value);
      } else if constexpr(is_string<MT>::value) {
	member_v = new_value;
      } else if constexpr(is_integral<MT>::value) {
	istringstream vin(new_value); vin >> member_v;
      } else if constexpr(is_floating_point<MT>::value) {
	istringstream vin(new_value); vin >> member_v;
      } else if constexpr(is_function<decltype(get_struct_descriptor<MT>)>::value) {
	auto m_sd = get_struct_descriptor<MT>();
	m_sd.set_value__(&member_v, new_value, path, curr_member_index + 1);
      } else {
	throw runtime_error(__func__);
    }
  }
    
  void get_value__(const any& o, vector<ValuePathValue>* out, ValuePath* path) override
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
  map<string, int> member_lookup;
  
  template <class MT, class T>
  void add_member(const char* member_name, MT T::*mptr)
  {
    member_names.push_back(member_name);
    member_lookup[member_name] = member_descriptors.size();
    member_descriptors.emplace_back(make_unique<MemberDescriptorT<MT, T>>(member_name, mptr));
  }

  void get_columns__(vector<ValuePath>* out, ValuePath* path)
  {
    for (auto& m_descr: member_descriptors) {
      m_descr->get_columns__(out, path);
    }
  }

  void get_columns(vector<string>* out)
  {
    ValuePath path;
    vector<ValuePath> pathes;
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
  
  void get_value__(const any& o, vector<ValuePathValue>* out, ValuePath* path)
  {
    for (auto& m_descr: member_descriptors) {
      m_descr->get_value__(o, out, path);
    }
  }

  void get_value(const any& o, vector<ValuePathValue>* out) {
    ValuePath path;
    get_value__(o, out, &path);
  }

  void set_value__(void* o, const string& new_value,
		   const ValuePath& path, size_t curr_index)
  {
    auto it = member_lookup.find(path[curr_index]);
    if (it == member_lookup.end()) {
      ostringstream m;
      m << "no such member: " << path[curr_index]
	<< ", full path: " << string_join(path, '.');
      throw runtime_error(m.str());
    }
    
    auto& md = member_descriptors[(*it).second];
    md->set_value__(o, new_value, path, curr_index);    
  }
  
  void set_value(void* o, const string& path_s, const string& new_value)
  {
    auto path = string_split(path_s, '.');
    set_value__(o, new_value, path, 0);
  }
};

#endif
