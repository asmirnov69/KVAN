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

template <> inline
StructDescriptor get_struct_descriptor<Ticker>() {
  static const StructDescriptor sd = {
    make_member_descriptor("ticker", &Ticker::ticker),
    make_member_descriptor("name", &Ticker::name),
    make_member_descriptor("sector", &Ticker::sector)
  };
  return sd;
}

#endif
