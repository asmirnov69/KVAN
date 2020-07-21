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
  void MemberDescriptorT<MT, T>::set_value__(void* o, const string& new_value,
				      const LOBKey& path, int curr_member_index)
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

template <class MT, class T> inline
  void MemberDescriptorT<MT, T>::visit_member_array_element(StructVisitor* visitor, LOBKey* curr_vpath, const any& o)
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

template <class MT, class T> inline
  void MemberDescriptorT<MT, T>::visit_member(StructVisitor* visitor, LOBKey* curr_vpath, const any& o)
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


