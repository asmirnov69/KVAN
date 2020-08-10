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

typedef optional<string> optional_string_t;
inline ostream& operator<<(ostream& out, const optional_string_t& v) {
  out << v ? v.value() : string("<null>");
  return out;
}
struct json_null_t {};

typedef vector<string> LOBKey;
typedef vector<int> mindex_t;
typedef vector<pair<string, mindex_t>> path_t;  // member, index
string to_string(const path_t&);

class StructVisitor
{
public:
  virtual void visit_null(const LOBKey& path) = 0;
  virtual void visit_key(const LOBKey& path) = 0;
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
  MemberDescriptor(const char* member_name);

  virtual void set_value__(void* o, const optional_string_t& new_value,
			   const path_t& path,
			   int curr_member_index) = 0;
  virtual void visit_member(StructVisitor* visitor,
			    LOBKey* curr_vpath,
			    const any& o) = 0;
};

template <class MT, class T>
class MemberDescriptorT : public MemberDescriptor
{
private:
  MT T::*mptr;

public:
  MemberDescriptorT(const char* member_name, MT T::*mptr);
  void set_value__(void* o, const optional_string_t& new_value,
		   const path_t& path, int curr_member_index) override;
  void visit_member(StructVisitor* visitor, LOBKey* curr_vpath,
		    const any& o) override;
};

template <class V>
class VectorDescriptorT
{
public:
  VectorDescriptorT();
  void set_value__(void* o, const optional_string_t& new_value,
		   const path_t& path, int curr_member_index, int dim_index);
  void visit_vector(StructVisitor* visitor, LOBKey* curr_vpath,
		    const V& o);
};

template <> class VectorDescriptorT<json_null_t> : public false_type {};

class StructDescriptor
{
public:
  void visit_members(StructVisitor* sv, LOBKey* curr_vpath, const any& o);
  void set_value__(void* o, const optional_string_t& new_value,
		   const path_t& path, int curr_index);

public:
  vector<string> member_names;
  vector<shared_ptr<MemberDescriptor>> member_descriptors;
  map<string, int> member_lookup;

  StructDescriptor();
  StructDescriptor(initializer_list<shared_ptr<MemberDescriptor>> l);
  void set_value(void* o, const path_t& path, const optional_string_t& new_value);
};

template <class T> inline
StructDescriptor get_struct_descriptor() {
  static_assert(assert_false<T>::value, "provide spec");
  return StructDescriptor();
}

template <class MT, class T> inline
shared_ptr<MemberDescriptor> make_member_descriptor(const char* n, MT T::* mptr)
{
  return make_shared<MemberDescriptorT<MT, T>>(n, mptr);
}

#include <kvan/struct-descriptor-inlines.h>

#endif
