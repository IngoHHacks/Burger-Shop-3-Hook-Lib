#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "bs3hooklib.h"

class ExampleHook : public HookBase {
    // Type definitions
    // bool __thiscall GameUpdate(MyApp* this, bool a2)
    typedef bool(__thiscall* tGameUpdateFunc)(void* thisPtr, bool a2);
    // void *__cdecl operator new(size_t size)
    typedef void* (__cdecl* tOperatorNew)(size_t size);
    // void __thiscall Customer::Customer(Customer* this)
    typedef void(__thiscall* tCustomerCtor)(void* thisPtr);
    // void __thiscall CustomerSlot::SpawnCustomer(CustomerSlot* this, Customer* customer, bool useQueue)
    typedef void(__thiscall* tSpawnCustomer)(void* thisPtr, void* customer, bool useQueue);

    static tGameUpdateFunc GameUpdateFunc;
    static tOperatorNew GameOperatorNew;
    static tCustomerCtor CustomerCtor;
    static tSpawnCustomer SpawnCustomer;

    // Name
    const std::string Name() override {
        return "Example Hook";
    }

    // Addresses
    LPVOID GetHookAddress() override {
        return UPDATE_OFFSET.AsPtr();
    }

    LPVOID GetHookFunction() override {
        return &HookedGameTick;
    }

    LPVOID* GetOriginalFunction() override {
        return reinterpret_cast<LPVOID*>(&GameUpdateFunc);
    }

    // Function offsets
    const MemPtr UPDATE_OFFSET = 0x256DB0;
    const MemPtr OPERATOR_NEW_OFFSET =  0x370993;
    const MemPtr CUSTOMER_CTOR_OFFSET = 0x63A70;
    const MemPtr CUSTOMER_SPAWN_OFFSET = 0x72570;

    // Variable offsets
    const std::vector<MemPtr> SLOT_OFFSET = { GetMyAppPointer(), GetMyAppBoardOffset(), 0xF0 , 0x0 };

    // Memory sizes
    const int CUSTOMER_SIZE = 0x1F0;

    // Function initialization
    void Init() override {
        MemPtr baseAddress = GetBaseAddress();
        GameOperatorNew = baseAddress + OPERATOR_NEW_OFFSET;
        if (!GameOperatorNew) {
            LogError("Could not find operator new.");
            return;
        }
        CustomerCtor = baseAddress + CUSTOMER_CTOR_OFFSET;
        if (!CustomerCtor) {
            LogError("Could not find customer constructor.");
            return;
        }
        SpawnCustomer = baseAddress + CUSTOMER_SPAWN_OFFSET;
        if (!SpawnCustomer) {
            LogError("Could not find spawn customer function.");
            return;
        }
    }

    // Hook
    static char __fastcall HookedGameTick(void* _this, void* _, bool a2) {

        // Do stuff here

        // Call the original tick function after
        return GameUpdateFunc(_this, a2);
    }

    // Commands
    bool Command(const StringNoCase& command) override {
        if (command == "spawn") {
            LogInfo("Spawning Customer...");
            DoSpawnCustomer();
            return true;
        }
        return false;
    }

    void PrintCommandHelp() override {
        std::cout << "spawn - Spawns a customer" << std::endl;
    }

    void* InstantiateCustomer() {
        void* customer = GameOperatorNew(CUSTOMER_SIZE);
        if (!customer) {
            LogError("Could not allocate memory for customer.");
            return nullptr;
        }
        CustomerCtor(customer);
        return customer;
    }

    void DoSpawnCustomer() {
        void* slot = nullptr;
        int slotNum = 0;
        HANDLE hProcess = GetProcess();
        do
        {
            MemPtr customer = ReadMemory<MemPtr>(hProcess, ReadSequenceAddress(hProcess, SLOT_OFFSET) + 0x10 + (slotNum * 40));
            bool closed = ReadMemory<bool>(hProcess, ReadSequenceAddress(hProcess, SLOT_OFFSET) + 0x14 + (slotNum * 40));
            if (customer == 0 && !closed) {
                slot = (void*)((ReadSequenceAddress(hProcess, SLOT_OFFSET)) + (slotNum * 40));
                break;
            }
            slotNum++;
        } while (slotNum < 4);
        if (!slot) {
            LogError("Could not find an open slot.");
            return;
        }

        void* cust = InstantiateCustomer();
        if (!cust) {
            LogError("Could not instantiate customer.");
            return;
        }
        LogInfo("Spawning customer at slot " + std::to_string(slotNum));
        SpawnCustomer(slot, cust, false);
        LogInfo("Customer spawned!");
    }
};

ExampleHook::tGameUpdateFunc ExampleHook::GameUpdateFunc = nullptr;
ExampleHook::tOperatorNew ExampleHook::GameOperatorNew = nullptr;
ExampleHook::tCustomerCtor ExampleHook::CustomerCtor = nullptr;
ExampleHook::tSpawnCustomer ExampleHook::SpawnCustomer = nullptr;

void RegisterHooks() {
    RegisterHook(new ExampleHook());
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