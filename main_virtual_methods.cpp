#include "dprint.cpp"
#include <iostream>

struct Granny {
  int g = 0x12;
  virtual void f() { std::cout << g << std::endl; }
};

struct Mom : virtual Granny {
  int m = 0x23;
};

struct Dad : virtual Granny {
  int d = 0x34;
};

struct Son : Mom, Dad {
  int s = 0x45;
  void f() override { std::cout << s << std::endl; }
};

int main() {
  std::cout << 55 << std::endl;
  // void (Son::*pf)() = &Son::h;
  // auto longptr = reinterpret_cast<long long &>(pf);
  // std::cout << longptr << std::endl;
  // auto first = *longptr;
  // auto second = *(longptr + 1);
  // std::cout << first << "  " << second << std::endl;
  Son s;
  print_object(s);
  std::cout << sizeof(s) << std::endl;
  std::cout << std::endl;
  Son *ps = &s;
  auto vtable_ptr = reinterpret_cast<void ***&>(ps);
  std::cout << "son|mom-in-son" << std::endl;
  print_memory(*vtable_ptr - 3, 64);
  std::cout << "dad-in-son" << std::endl;
  print_memory(*(vtable_ptr + 2) - 3, 64);
  std::cout << "granny-in-son" << std::endl;
  print_memory(*(vtable_ptr + 4) - 3, 64);
  std::cout << "son f" << std::endl;
  print_memory(**vtable_ptr, 64);
  std::cout << "granny f" << std::endl;
  print_memory(**(vtable_ptr + 4), 64);
  // std::cout << std::endl;
  // print_memory(*(vtable_ptr + 1), 32);
  // print_memory(v, 10);
}
