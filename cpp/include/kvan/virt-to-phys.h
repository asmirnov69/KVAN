// -*- c++ -*-
// from https://stackoverflow.com/a/45128487/1181482
#ifndef __KVAN__VIRT_TO_PHYS_HH__
#define __KVAN__VIRT_TO_PHYS_HH__

#include <unistd.h>

bool virt_to_phys_user(void** addr, pid_t pid, uintptr_t vaddr);
void* get_paddr(void* addr, pid_t pid = getpid());

#endif
