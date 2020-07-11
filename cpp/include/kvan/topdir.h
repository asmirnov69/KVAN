// -*- c++ -*-
#ifndef __KVAN__TOP_DIR__HH__
#define __KVAN__TOP_DIR__HH__

#include <string>
using namespace std;

#include <kvan/only.h>

struct TopDirImpl {
  TopDirImpl(const char* marker_file = "TOP-DIR");
  string get_topdir() { return topdir; }  

private:
  string topdir;  
};

typedef Only<TopDirImpl> TopDir;

#endif
