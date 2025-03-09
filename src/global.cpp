#include "global.h"

#include <vector>

HANDLE hProcess = nullptr;
DWORD pid = 0;
MemPtr baseAddress = 0;

const MemPtr MYAPP_POINTER = 0x45A330;
const MemPtr MYAPP_BOARD_OFFSET = 0x778;

std::vector<HookBase*> hooks = std::vector<HookBase*>();