// sudo ./page-types -p 22341 -l -N | less
//
#include <iostream>
using namespace std;

#include <kvan/virt-to-phys.h>
#include <unistd.h>

int main()
{
  cout << "pid: " << getpid() << endl;
  char* b = new char[1024 * 10];
  void* pa;
  
  if (!virt_to_phys_user(&pa, getpid(), uintptr_t(b))) {
    cerr << "virt_to_phys_user failed" << endl;
    return 1;
  }
  
  cout << (void*)b << " " << pa << endl;
  cout << (void*)b << " " << get_paddr(b) << endl;
  pause();
  return 0;
}
