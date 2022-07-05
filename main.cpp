#include <iostream>
#include <Windows.h>


void custom_print() {
    std::cout << "custom print called.\n";
}

long __stdcall exception_handler(EXCEPTION_POINTERS* exception_info) {
    std::cout << "custom vectored exception handler called.\n";
    if (exception_info->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION && exception_info->ContextRecord->Eip == reinterpret_cast<uintptr_t>(&Sleep)) {
        std::cout << "hooked sleep, changing EIP to custom_print's address.\n";
        
        exception_info->ContextRecord->Eip = reinterpret_cast<uintptr_t>(&custom_print);
        exception_info->ContextRecord->EFlags |= 0x100;

        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else if (exception_info->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) {
        DWORD vp_old_pt;
        std::cout << "resetting window's sleep function page access flags to PAGE_EXECUTE_READ and PAGE_GUARD.\n";
        VirtualProtect(&Sleep, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &vp_old_pt);

        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int main() {

    const auto veh_handle = AddVectoredExceptionHandler(true, &exception_handler);

    if (veh_handle == nullptr) { 
        printf("%s", "failed to add vectored exception handler.");
        return -1;
    }


    DWORD vp_old_pt;
    VirtualProtect(&Sleep, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &vp_old_pt);

    std::cout << "calling window's sleep function\n";

    Sleep(5000);
    std::cin.get();
    return 0;
}
