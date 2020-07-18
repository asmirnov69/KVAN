// -*- c++ -*-
#ifndef __KVAN_STRUCT_DESCRIPTOR__HH__
#define __KVAN_STRUCT_DESCRIPTOR__HH__

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
#include <kvan/lob.h>

struct StructDescriptor;
template <class T> StructDescriptor get_struct_descriptor();

class StructVisitor
{
public:
  virtual void visit_enum(const LOBKey& path, const string& enum_s) = 0;
  virtual void visit_string(const LOBKey& path, const string& s) = 0;
  virtual void visit_fundamental(const LOBKey& path, const string& v) = 0;
  virtual void visit_start_map(const LOBKey& path) = 0;
  virtual void visit_end_map() = 0;
  virtual void visit_delimiter() = 0;
  virtual void visit_start_array() = 0;
  virtual void visit_end_array() = 0;
};

class MemberDescriptor
{
public:
  string member_name;
  MemberDescriptor(const char* member_name) {
    this->member_name = member_name;
  }

  virtual void collect_value_pathes__(LOBKey* curr_vpath,
				      vector<LOBKey>* out) = 0;
  virtual void collect_values__(const any&, LOBKey* curr_vpath,
				vector<LOBKeyValue>* out) = 0;
  virtual void set_value__(void* o, const string& new_value,
			   const LOBKey& path,
			   int curr_member_index) = 0;
  virtual void visit_member(StructVisitor* visitor,
			    LOBKey* curr_vpath,
			    const any& o) = 0;
  virtual void visit_member_array_element(StructVisitor* visitor,
					  LOBKey* curr_vpath,
					  const any& o) = 0;
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

  void collect_value_pathes__(LOBKey* curr_vpath, vector<LOBKey>* out) override
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
		   const LOBKey& path, int curr_member_index) override
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
    
  void collect_values__(const any& o, LOBKey* curr_vpath,
			vector<LOBKeyValue>* out) override
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

  void visit_member_array_element(StructVisitor* visitor, LOBKey* curr_vpath, const any& o) override
  {
    if constexpr(is_vector<MT>::value) {
	typedef typename MT::value_type vt;
	try {
	  const vt& member_v = any_cast<vt>(o);
	  if constexpr(is_enum<vt>::value) {
	      visitor->visit_enum(*curr_vpath, get_enum_value_string<vt>(member_v));
	    } else if constexpr(is_string<vt>::value) {
	      visitor->visit_string(*curr_vpath, member_v);
	    } else if constexpr(is_fundamental<vt>::value) {
	      visitor->visit_fundamental(*curr_vpath, to_string(member_v));	
	    } else if constexpr(is_vector<vt>::value) {
	      visitor->visit_start_array();
	      for (size_t i = 0; i < member_v.size(); i++) {
		this->visit_member_array_element(visitor, curr_vpath, member_v[i]);
	      }
	      visitor->visit_end_array();
	    } else if constexpr(is_function<decltype(get_struct_descriptor<vt>)>::value) {
	      auto m_sd = get_struct_descriptor<vt>();
	      m_sd.visit_members(visitor, curr_vpath, member_v);
	    } else {
	    throw runtime_error(__func__);
	  }
	} catch (const bad_any_cast& ex) {
	  throw runtime_error(ex.what());
	}
      }
  }
  
  void visit_member(StructVisitor* visitor, LOBKey* curr_vpath, const any& o) override
  {
    try {
      const T& obj = any_cast<T>(o);
      const MT& member_v = obj.*mptr;

      curr_vpath->push_back(member_name);
      if constexpr(is_enum<MT>::value) {
	  visitor->visit_enum(*curr_vpath, get_enum_value_string<MT>(member_v));	
      } else if constexpr(is_string<MT>::value) {
	  visitor->visit_string(*curr_vpath, member_v);
      } else if constexpr(is_fundamental<MT>::value) {
	  visitor->visit_fundamental(*curr_vpath, to_string(member_v));	
      } else if constexpr(is_vector<MT>::value) {	  
	  visitor->visit_start_array();
	  for (size_t i = 0; i < member_v.size(); i++) {
	    this->visit_member_array_element(visitor, curr_vpath, member_v[i]);
	  }
	  visitor->visit_end_array();
      } else if constexpr(is_function<decltype(get_struct_descriptor<MT>)>::value) {
	auto m_sd = get_struct_descriptor<MT>();
	m_sd.visit_members(visitor, curr_vpath, member_v);
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
  void visit_members(StructVisitor* sv, LOBKey* curr_vpath, const any& o)
  {
    sv->visit_start_map(*curr_vpath);
    for (size_t i = 0; i < member_descriptors.size(); i++) {
      auto m_descr = member_descriptors[i];
      m_descr->visit_member(sv, curr_vpath, o);
      if (i + 1 < member_descriptors.size()) {
	sv->visit_delimiter();
      }
    }
    sv->visit_end_map();
  }
  
  void collect_value_pathes__(LOBKey* curr_vpath, vector<LOBKey>* out)
  {
    for (auto& m_descr: member_descriptors) {
      m_descr->collect_value_pathes__(curr_vpath, out);
    }
  }  

  void collect_values__(const any& o, LOBKey* curr_vpath,
			vector<LOBKeyValue>* out)
  {
    for (auto& m_descr: member_descriptors) {
      m_descr->collect_values__(o, curr_vpath, out);
    }
  }

  void set_value__(void* o, const string& new_value,
		   const LOBKey& path, size_t curr_index)
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
  
  vector<LOBKey> get_value_pathes()
  {
    LOBKey path;
    vector<LOBKey> pathes;
    collect_value_pathes__(&path, &pathes);
    return pathes;
  }
  
  vector<LOBKeyValue> get_values(const any& o)
  {
    vector<LOBKeyValue> out;
    LOBKey path;
    collect_values__(o, &path, &out);
    return out;
  }

  void set_value(void* o, const LOBKey& path, const string& new_value)
  {
    set_value__(o, new_value, path, 0);
  }
};

template <class T> inline void from_LOB(T* obj, const LOB& lob)
{
  auto sd = get_struct_descriptor<T>();
  for (auto& kv: lob) {
    sd.set_value(obj, kv.first, kv.second);
  }
}
    
template <class T> inline void to_LOB(LOB* lob, const T& obj)
{
  auto sd = get_struct_descriptor<T>();
  lob->set(sd.get_values(obj));
}

#endif
