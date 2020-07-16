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

template <class T> void to_json(ostream& out, const T& v) {
  throw runtime_error(__func__);
}

class MemberDescriptor
{
public:
  string member_name;
  MemberDescriptor(const char* member_name) {
    this->member_name = member_name;
  }

  virtual void collect_value_pathes__(ValuePath* curr_vpath,
				      vector<ValuePath>* out) = 0;
  virtual void collect_values__(const any&, ValuePath* curr_vpath,
				vector<ValuePathValue>* out) = 0;
  virtual void set_value__(void* o, const string& new_value,
			   const ValuePath& path,
			   int curr_member_index) = 0;
  virtual void to_json__(ostream& out, const any& o) = 0;
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

// https://stackoverflow.com/a/31105859/1181482
template <typename T> struct is_vector: std::false_type {};
template <typename... Args> struct is_vector<std::vector<Args...>> : std::true_type{};

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

  void collect_value_pathes__(ValuePath* curr_vpath, vector<ValuePath>* out) override
  {
    curr_vpath->push_back(member_name);
    if constexpr(is_enum<MT>::value) {
	out->push_back(*curr_vpath);
    } else if constexpr(is_string<MT>::value) {
	out->push_back(*curr_vpath);
    } else if constexpr(is_fundamental<MT>::value) {
	out->push_back(*curr_vpath);
      } else if constexpr(is_vector<MT>::value) {
	throw runtime_error(__func__);
      } else if constexpr(is_function<decltype(get_struct_descriptor<MT>)>::value) {
      auto sd = get_struct_descriptor<MT>();
      sd.collect_value_pathes__(curr_vpath, out);
    } else {
      throw runtime_error(__func__);
    }
    curr_vpath->pop_back();
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
      } else if constexpr(is_fundamental<MT>::value) {
	istringstream vin(new_value); vin >> member_v;
      } else if constexpr(is_vector<MT>::value) {
	throw runtime_error(__func__);
      } else if constexpr(is_function<decltype(get_struct_descriptor<MT>)>::value) {
	auto m_sd = get_struct_descriptor<MT>();
	m_sd.set_value__(&member_v, new_value, path, curr_member_index + 1);
      } else {
	throw runtime_error(__func__);
    }
  }
    
  void collect_values__(const any& o, ValuePath* curr_vpath, vector<ValuePathValue>* out) override
  {
    try {
      const T& obj = any_cast<T>(o);
      const MT& member_v = obj.*mptr;

      curr_vpath->push_back(member_name);
      if constexpr(is_enum<MT>::value) {
	  out->push_back(make_pair(*curr_vpath, "\"" + get_enum_value_string<MT>(member_v) + "\""));
      } else if constexpr(is_string<MT>::value) {
	  out->push_back(make_pair(*curr_vpath, "\"" + member_v + "\""));
      } else if constexpr(is_fundamental<MT>::value) {
	  out->push_back(make_pair(*curr_vpath, to_string(member_v)));
      } else if constexpr(is_vector<MT>::value) {
	throw runtime_error(__func__);
      } else if constexpr(is_function<decltype(get_struct_descriptor<MT>)>::value) {
	auto m_sd = get_struct_descriptor<MT>();
	m_sd.collect_values__(member_v, curr_vpath, out);
      } else {
	throw runtime_error(__func__);
      }
      curr_vpath->pop_back();
    } catch (const bad_any_cast& ex) {
      throw runtime_error(ex.what());      
    }
  }

  void to_json__(ostream& out, const any& o) override {
    try {
      const T& obj = any_cast<T>(o);
      const MT& member_v = obj.*mptr;
      out << "\"" << member_name << "\": ";
      to_json<MT>(out, member_v);
    } catch (const bad_any_cast& ex) {
      throw runtime_error(ex.what());      
    }
  }
};

class StructDescriptor
{
public:
  void collect_value_pathes__(ValuePath* curr_vpath, vector<ValuePath>* out)
  {
    for (auto& m_descr: member_descriptors) {
      m_descr->collect_value_pathes__(curr_vpath, out);
    }
  }  

  void collect_values__(const any& o, ValuePath* curr_vpath,
			vector<ValuePathValue>* out)
  {
    for (auto& m_descr: member_descriptors) {
      m_descr->collect_values__(o, curr_vpath, out);
    }
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

  void to_json__(ostream& out, const any& o)
  {
    out << "{";
    for (size_t i = 0; i < member_descriptors.size(); ++i) {
      member_descriptors[i]->to_json__(out, o);
      if (i + 1 < member_descriptors.size()) {
	out << ", ";
      }
    }
    out << "}";
  }

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

  vector<ValuePath> get_value_pathes()
  {
    ValuePath path;
    vector<ValuePath> pathes;
    collect_value_pathes__(&path, &pathes);
    return pathes;
  }
  
  vector<ValuePathValue> get_values(const any& o)
  {
    vector<ValuePathValue> out;
    ValuePath path;
    collect_values__(o, &path, &out);
    return out;
  }

  void set_value(void* o, const string& path_s, const string& new_value)
  {
    auto path = string_split(path_s, '.');
    set_value__(o, new_value, path, 0);
  }

  void to_json(ostream& out, const any& o)
  {
    to_json__(out, o);
  }
};

#endif
