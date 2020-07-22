// -*- c++ -*-

inline MemberDescriptor::MemberDescriptor(const char* member_name)
{
  this->member_name = member_name;
}

template <class MT, class T> inline
MemberDescriptorT<MT, T>::MemberDescriptorT(const char* member_name, MT T::*mptr) :
  MemberDescriptor(member_name)
{
  this->mptr = mptr;
}

template <class MT, class T> inline
void MemberDescriptorT<MT, T>::set_value__(void* o,
					   const string& new_value,
					   const LOBKey& path,
					   int curr_member_index)
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
    static_assert(assert_false<T>::value, "void ScalarMemberDescriptorT<MT, T>::set_value__ can't decide on MT");
  }
}

template <class MT, class T> inline
void MemberDescriptorT<MT, T>::visit_member(StructVisitor* visitor, LOBKey* curr_vpath, const any& o)
{
  try {
    const T& obj = any_cast<T>(o);
    const MT& member_v = obj.*mptr;
    
    curr_vpath->push_back(member_name);
    visitor->visit_key(*curr_vpath);
    if constexpr(is_enum<MT>::value) {
	visitor->visit_enum(*curr_vpath, get_enum_value_string<MT>(member_v));	
      } else if constexpr(is_string<MT>::value) {
	visitor->visit_string(*curr_vpath, member_v);
      } else if constexpr(is_fundamental<MT>::value) {
	visitor->visit_fundamental(*curr_vpath, to_string(member_v));
      } else if constexpr(is_vector<MT>::value) {
	VectorDescriptorT<MT> vd;
	vd.visit_vector(visitor, curr_vpath, member_v);
      } else if constexpr(is_function<decltype(get_struct_descriptor<MT>)>::value) {
	auto m_sd = get_struct_descriptor<MT>();
	m_sd.visit_members(visitor, curr_vpath, member_v);
      } else {
      static_assert(assert_false<T>::value, "void ScalarMemberDescriptorT<MT, T>::visit_member can't decide on MT");
    }
    curr_vpath->pop_back();
  } catch (const bad_any_cast& ex) {
    throw runtime_error(ex.what());
  }
}

template <class V> inline
VectorDescriptorT<V>::VectorDescriptorT()
{
}

template <class V> inline
void VectorDescriptorT<V>::set_value__(void* o, const string& new_value,
				       const LOBKey& path,
				       int curr_member_index)
{
  throw runtime_error(__func__);
}

template <class V> inline
void VectorDescriptorT<V>::visit_vector(StructVisitor* visitor,
					 LOBKey* curr_vpath,
					 const V& v)
{
  typedef typename V::value_type vt;
  visitor->visit_start_array();
  for (size_t i = 0; i < v.size(); i++) {
    auto& el = v[i];
    if constexpr(is_enum<vt>::value) {
	visitor->visit_enum(*curr_vpath, get_enum_value_string<vt>(el));
      } else if constexpr(is_string<vt>::value) {
      	visitor->visit_string(*curr_vpath, el);
      } else if constexpr(is_fundamental<vt>::value) {
	visitor->visit_fundamental(*curr_vpath, to_string(el));
      } else if constexpr(is_vector<vt>::value) {
	VectorDescriptorT<vt> vd;
	vd.visit_vector(visitor, curr_vpath, el);
      } else if constexpr(is_function<decltype(get_struct_descriptor<vt>)>::value) {
	StructDescriptor sd = get_struct_descriptor<vt>();
	sd.visit_members(visitor, curr_vpath, el);	      
      } else {
      static_assert(assert_false<vt>::value, "void VectorDescriptorT<VT>::visit_vector can't decide on VT");
    }

    if (i + 1 < v.size()) {
      visitor->visit_delimiter();
    }
  }
  visitor->visit_end_array();
}
