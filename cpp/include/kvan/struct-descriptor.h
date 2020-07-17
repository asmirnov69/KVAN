// -*- c++ -*-
#ifndef __RQX_STRUCT_DESCRIPTOR__HH__
#define __RQX_STRUCT_DESCRIPTOR__HH__

#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <any>
#include <initializer_list>
using namespace std;

#include <kvan/addl-type-traits.h>
#include <kvan/enum-io.h>
#include <kvan/string-utils.h>

struct StructDescriptor;
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

  virtual void collect_value_pathes__(ValuePath* curr_vpath,
				      vector<ValuePath>* out) = 0;
  virtual void collect_values__(const any&, ValuePath* curr_vpath,
				vector<ValuePathValue>* out) = 0;
  virtual void set_value__(void* o, const string& new_value,
			   const ValuePath& path,
			   int curr_member_index) = 0;

  virtual void to_json__(ostream& out, const any& o) const = 0;
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

  void to_json__(ostream& out, const any& o) const override;

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
};

template <class MT, class T> inline
shared_ptr<MemberDescriptor> make_member_descriptor(const char* n, MT T::* mptr)
{
  return make_shared<MemberDescriptorT<MT, T>>(n, mptr);
}

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

public:
  StructDescriptor(initializer_list<shared_ptr<MemberDescriptor>> l) {
    for (auto el: l) {
      member_names.push_back(el->member_name);
      member_lookup[el->member_name] = member_descriptors.size();
      member_descriptors.push_back(el);
    }
  }
  vector<string> member_names;
  vector<shared_ptr<MemberDescriptor>> member_descriptors;
  map<string, int> member_lookup;
  
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
};

#endif
