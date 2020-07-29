// -*- c++ -*-
#ifndef __KVAN_UUID__HH__
#define __KVAN_UUID__HH__

// from https://stackoverflow.com/a/60198074/1181482
//
#include <random>
#include <sstream>

namespace uuid {
  static std::random_device              rd;
  static std::mt19937                    gen(rd());
  static std::uniform_int_distribution<> dis(0, 15);
  static std::uniform_int_distribution<> dis2(8, 11);
  
  std::string generate_uuid_v4();
}

#endif
