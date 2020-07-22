#include <kvan/struct-descriptor.h>

string to_string(const path_t& p)
{
  ostringstream out;
  for (size_t i = 0; i < p.size(); i++) {
    out << p[i].first;
    if (p[i].second.size() > 0) {
      out << "[";
      for (size_t j = 0; j < p[i].second.size(); j++) {
	out << p[i].second[j];
	if (j + 1 < p[i].second.size()) {
	  out << ",";
	}
      }
      out << "]";
    }
    if (i + 1 < p.size()) {
      out << ".";
    }
  }
  return out.str();
}
  
StructDescriptor::StructDescriptor()
{
}

StructDescriptor::StructDescriptor(initializer_list<shared_ptr<MemberDescriptor>> l)
{
  for (auto el: l) {
    member_names.push_back(el->member_name);
    member_lookup[el->member_name] = member_descriptors.size();
    member_descriptors.push_back(el);
  }
}

void StructDescriptor::visit_members(StructVisitor* sv, LOBKey* curr_vpath, const any& o)
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

void StructDescriptor::set_value__(void* o, const string& new_value,
				   const path_t& path, int curr_index)
{
  auto it = member_lookup.find(path[curr_index].first);
  if (it == member_lookup.end()) {
    ostringstream m;
    m << "no such member: " << path[curr_index].first
      << ", full path: " << to_string(path);
    throw runtime_error(m.str());
  }
  
  auto& md = member_descriptors[(*it).second];
  md->set_value__(o, new_value, path, curr_index);
}

void StructDescriptor::set_value(void* o, const path_t& path, const string& new_value)
{
  // first element of dpath is ?this, discard
  set_value__(o, new_value, path, 1);
}
