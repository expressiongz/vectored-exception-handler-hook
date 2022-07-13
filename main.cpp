#include <iostream>
#include <Windows.h>

void print() {
  std::cout << "hello!\n";
}

long __stdcall handler(EXCEPTION_POINTERS * exception) {
  if (exception -> ExceptionRecord -> ExceptionCode == STATUS_GUARD_PAGE_VIOLATION && exception -> ContextRecord -> Eip == reinterpret_cast < std::uint32_t > ( & Sleep)) {
    exception -> ContextRecord -> Eip = reinterpret_cast < std::uint32_t > ( & print);
    DWORD vp_old_protection{ 0 };
    VirtualProtect( & Sleep, 1, PAGE_EXECUTE_READ | PAGE_GUARD, & vp_old_protection);
    return EXCEPTION_CONTINUE_EXECUTION;
  }
  return EXCEPTION_CONTINUE_SEARCH;
}

int main() {
  AddVectoredExceptionHandler(true, & handler);
  DWORD vp_old_protection{ 0 };
  VirtualProtect( & Sleep, 1, PAGE_EXECUTE_READ | PAGE_GUARD, & vp_old_protection);
  Sleep(1000);
  return 0;
}
