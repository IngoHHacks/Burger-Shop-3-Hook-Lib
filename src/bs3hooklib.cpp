#include "bs3hooklib.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

#include "ansicolor.h"
#include "MinHook.h"
#include "global.h"

const std::string HookBase::Name() { return "Unnamed Mod"; }
void HookBase::Init() {}
void HookBase::Hook() {
    LPVOID hookAddress = baseAddress + GetHookAddress();
    LPVOID hookFunction = GetHookFunction();
    LPVOID* originalFunction = GetOriginalFunction();
    std::cout << FormatNameLevel("CORE", "INFO") << "Hooking " << std::hex << hookAddress << " to " << hookFunction << std::endl;
    if (MH_CreateHook(hookAddress, hookFunction, originalFunction) != MH_OK) {
        std::cerr << FormatNameLevel("CORE", "ERROR") << "Failed to create hook." << std::endl;
        return;
    }
    if (MH_EnableHook(hookAddress) != MH_OK) {
        std::cerr << FormatNameLevel("CORE", "ERROR") << "Failed to enable hook." << std::endl;
        return;
    }
}
bool HookBase::Command(const StringNoCase& command) { return false; }

void HookBase::PrintCommandHelp() {
    std::cout << "No commands available for this mod." << std::endl;
}

void HookBase::Log(const std::string& message, const int level) {
    switch (level) {
        case LOG_INFO:
            LogInfo(message);
            break;
        case LOG_WARNING:
            LogWarning(message);
            break;
        case LOG_ERROR:
            LogError(message);
            break;
        default:
            std::cerr << FormatNameLevel(Name(), "???") << message << std::endl;
    }
}

void HookBase::LogInfo(const std::string& message) {
    std::cout << FormatNameLevel(Name(), "INFO") << message << std::endl;
}

void HookBase::LogWarning(const std::string& message) {
    std::cout << YELLOW << FormatNameLevel(Name(), "WARNING") << message << CLEAR << std::endl;
}

void HookBase::LogError(const std::string& message) {
    std::cerr << FormatNameLevel(Name(), "ERROR") << message << std::endl;
}

void RegisterHook(HookBase *hook) {
    hook->Init();
    hook->Hook();
    hooks.push_back(hook);
    std::cout << FormatNameLevel("CORE", "INFO") << "Registered " << hook->Name() << std::endl;
}

HANDLE GetProcess () { return hProcess; }
DWORD GetPID () { return pid; }
MemPtr GetBaseAddress () { return baseAddress; }
const MemPtr GetMyAppPointer () { return MYAPP_POINTER; }
const MemPtr GetMyAppBoardOffset () { return MYAPP_BOARD_OFFSET; }