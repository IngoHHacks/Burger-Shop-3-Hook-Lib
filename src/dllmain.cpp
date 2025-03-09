#include "dllmain.h"

#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>

#include "MinHook.h"
#include "global.h"
#include "versionproxy.h"
#include <algorithm>

void GetGameHandle() {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to create snapshot! Error: " + std::to_string(GetLastError()));
		return;
	}
	pid = 0;
	if (Process32First(hSnap, &entry)) {
		do {
			if (!_wcsicmp(entry.szExeFile, L"BurgerShop3.exe")) {
				std::cout << FormatNameLevel("CORE", "INFO") << "Attaching to BurgerShop3.exe, PID: " << entry.th32ProcessID << std::endl;
				pid = entry.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnap, &entry));
	}
	else {
		CloseHandle(hSnap);
		throw std::runtime_error("Failed to find process! Error: " + std::to_string(GetLastError()));
		return;
	}
	CloseHandle(hSnap);
	if (pid == 0) {
		throw std::runtime_error("Failed to find process!");
		return;
	}

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess) {
		throw std::runtime_error("Failed to open process! Error: " + std::to_string(GetLastError()));
		return;
	}
	std::cout << FormatNameLevel("CORE", "INFO") << "Attached to Burger Shop 3!" << std::endl;

	HANDLE hSnap2 = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hSnap2 == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to create snapshot! Error: " + std::to_string(GetLastError()));
		return;
	}
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnap2, &modEntry)) {
		do {
			if (!_wcsicmp(modEntry.szModule, L"BurgerShop3.exe")) {
				baseAddress = (MemPtr)modEntry.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnap2, &modEntry));
	}
	CloseHandle(hSnap2);
	if (baseAddress == 0) {
		throw std::runtime_error("Failed to find base address!");
		return;
	}
}

void CreateConsole() {
	AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
	freopen_s(&f, "CONIN$", "r", stdin);
}

void CloseConsole() {
	FreeConsole();
}

bool HookCommand(const std::string& command) {
	bool handled = false;
	for (auto hook : hooks) {
		if (hook->Command(command)) {
			handled = true;
			break;
		}
	}
	return handled;
}

DWORD WINAPI ConsoleLoop(LPVOID param) {
    std::string command;
    while (true) {
        std::cout << "> ";
		command.clear();
		if (!std::getline(std::cin, command)) {
			std::cerr << FormatNameLevel("CORE", "ERROR") << "Failed to read console!";
			break;
		}
        if (HookCommand(command)) {
			continue;
        }
        else if (command == "exit") {
            std::cout << FormatNameLevel("CORE", "INFO") << "Unloading DLL...\n";
			CloseConsole();
			CloseHandle(hProcess);
			hProcess = nullptr;
            FreeLibraryAndExitThread(GetModuleHandle(NULL), 0);
		}
		else if (command == "help") {
			std::cout << "Available commands:\n";
			std::cout << "exit - Unloads the DLL\n";
			for (auto hook : hooks) {
				std::cout << hook->Name() << " Commands:\n";
				hook->PrintCommandHelp();
			}
		}
		else if (command.empty()) {
			return 0;
        }
        else {
            std::cout << "Unknown command: " << command << "\n";
        }
    }
    return 0;
}

std::vector<HMODULE> libs = std::vector<HMODULE>();

void LoadHooks() {
	if (MH_Initialize() != MH_OK) {
		throw std::runtime_error("Failed to initialize MinHook!");
		return;
	}
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(L"mods\\*.dll", &findFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		std::cout << FormatNameLevel("CORE", "INFO") << "No mods found!" << std::endl;
		return;
	}
	std::cout << FormatNameLevel("CORE", "INFO") << "Loading mods..." << std::endl;
	do {
		std::wstring path = L"mods\\" + std::wstring(findFileData.cFileName);
		HMODULE lib = LoadLibrary(path.c_str());
		if (lib) {
			std::string prefix = FormatNameLevel("CORE", "INFO");
			std::wcout << std::wstring(prefix.begin(), prefix.end()) << L"Loaded " << findFileData.cFileName << std::endl;
			libs.push_back(lib);
		}
	} while (FindNextFile(hFind, &findFileData));
	FindClose(hFind);
}

bool IsModuleLoaded(const wchar_t* moduleName) {
	return GetModuleHandleW(moduleName) != nullptr;
}

DWORD WINAPI MainThread(LPVOID param) {
	while (!IsModuleLoaded(L"bass24.dll")) {
		Sleep(100);
	}
    CreateConsole();
	GetGameHandle();
	LoadHooks();
	CreateThread(nullptr, 0, ConsoleLoop, nullptr, 0, nullptr);
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       MemPtr lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
		LoadRealVersionDLL();
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }
	else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		CloseHandle(hProcess);
		hProcess = nullptr;
		for (auto lib : libs) {
			FreeLibrary(lib);
		}
		FreeIfLoaded();
	}
    return TRUE;
}