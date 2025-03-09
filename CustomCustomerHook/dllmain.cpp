#include <Windows.h>
#include <iostream>
#include <string>
#include <map>

#include "bs3hooklib.h"

class CustomCustomerHook;

class CustomCustomerHook : public HookBase {
    // Types
    struct StringLessNoCase 
    { 
        bool operator()(const std::string &s1, const std::string &s2) const { return _stricmp(s1.c_str(),s2.c_str())<0; } 
    };

    // Aliases
    using StringIntMapNoCase = std::map<std::string, int, StringLessNoCase>;

    // Type definitions
    // static StringIntMapNoCase& DefineCustomers()
    typedef StringIntMapNoCase& (* tGetCustomerMap)();
    static tGetCustomerMap GetCustomerMap;

    // Name
    const std::string Name() override {
        return "Cust Hook";
    }

    // Addresses
    LPVOID GetHookAddress() override {
        return DEFINE_CUSTOMERS.AsPtr();
    }

    LPVOID GetHookFunction() override {
        return &DefineExtraCustomers;
    }

    LPVOID* GetOriginalFunction() override {
        return reinterpret_cast<LPVOID*>(&GetCustomerMap);
    }
    
    // Function offsets
    const MemPtr DEFINE_CUSTOMERS = 0x371F0;

    // Hook
    static StringIntMapNoCase& __fastcall DefineExtraCustomers() {
        StringIntMapNoCase& custTree = GetCustomerMap();
        if (custTree.find("BigBurgers") == custTree.end()) {
            custTree["BigBurgers"] = 426;
        }
        return custTree;
    }
};

CustomCustomerHook::tGetCustomerMap CustomCustomerHook::GetCustomerMap = nullptr;

void RegisterHooks() {
    RegisterHook(new CustomCustomerHook());
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        RegisterHooks();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}