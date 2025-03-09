#ifndef GLOBAL_H
#define GLOBAL_H

#include "bs3hooklib.h"

#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

extern HANDLE hProcess;
extern DWORD pid;
extern MemPtr baseAddress;

extern const MemPtr MYAPP_POINTER;
extern const MemPtr MYAPP_BOARD_OFFSET;

extern std::vector<HookBase*> hooks;

#endif