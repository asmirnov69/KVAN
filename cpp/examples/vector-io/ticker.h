// -*- c++ -*-
#ifndef __KVAN_TICKER_HH__
#define __KVAN_TICKER_HH__

#include <kvan/struct-descriptor.h>

struct Ticker
{
  int sid{-1};
  string ticker;
  string name;
  string sector;
};

template <>
StructDescriptor get_struct_descriptor<Ticker>() {
  StructDescriptor sd;
  sd.add_member("ticker", &Ticker::ticker);
  sd.add_member("name", &Ticker::name);
  sd.add_member("sector", &Ticker::sector);
  return sd;
}

#endif
