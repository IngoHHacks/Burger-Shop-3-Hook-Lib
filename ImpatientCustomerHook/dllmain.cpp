#include "bs3hooklib.h"

#include <Windows.h>

struct CustomerFragment {
    char padding1[222]; // 222 unknown bytes
    int mAngerTime1;
    int mAngerTime2;
    int mAngerTime3;
    int mAngerTime4;
    int mAngerTime5;
    char padding2[152]; // 152 unknown bytes
    int mNumPatienceBars;
    // Padding afterwards is not needed as we're just writing
};

class ImpatientCustomersHook : public HookBase {
public:
    typedef void(__thiscall* tAfterSpawnCustomer)(void* thisPtr, void* customer);
    static tAfterSpawnCustomer AfterSpawnCustomer;

    const std::string Name() override {
        return "Impatient Customers Hook";
    }

    LPVOID GetHookAddress() override {
        return (LPVOID)0x1AB40; // Address of the spawn function
    }

    LPVOID GetHookFunction() override {
        return &MakeImpatient; // Pointer to the hook function
    }

    LPVOID* GetOriginalFunction() override {
        return reinterpret_cast<LPVOID*>(&AfterSpawnCustomer); // Pointer to the original function
    }

    static void __fastcall MakeImpatient(void* thisPtr, void* _, void* customer) {
        if (!customer) {
            return;
        }
        CustomerFragment* customerFragment = (CustomerFragment*)customer;

        // Set the anger times to 100*n
        customerFragment->mAngerTime1 = 100;
        customerFragment->mAngerTime2 = 200;
        customerFragment->mAngerTime3 = 300;
        customerFragment->mAngerTime4 = 400;
        customerFragment->mAngerTime5 = 500;

        // Set the number of patience bars to 1
        customerFragment->mNumPatienceBars = 1;

        // Call the original function
        AfterSpawnCustomer(thisPtr, customer);
    }
};

ImpatientCustomersHook::tAfterSpawnCustomer ImpatientCustomersHook::AfterSpawnCustomer = nullptr;


void RegisterHooks() {
    RegisterHook(new ImpatientCustomersHook());
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