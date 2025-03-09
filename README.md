# Burger Shop 3 Hooking Library

## Description
This is a C++ hooking library for Burger Shop 3.  
It uses a `version.dll` proxy and `MinHook` for hooking.  
The solution also contains three example hooks.
If you want to learn more about hooking, see [GUIDE.md]() and the [MinHook wiki](https://github.com/TsudaKageyu/minhook/wiki)
The library is for Windows only. Other operating systems are not supported. Feel free to make a pull request if you want to add support for other operating systems.

## Releases
Releases consist of three zip files:
- `bs3hooklib.zip` - The hooking library. Copy the `version.dll` to the game's directory.
- `bs3hooklib-dev.zip` - The development files. Includes the header file and the library file.
- `bs3hooklib-examples.zip` - The example hooks. Copy the DLLs to the `mods` directory in the game's directory to test them.

## Usage
- Copy the `version.dll` from the releases to the game's directory.  
- Put any custom hooks in the `mods` directory. (Create it if it doesn't exist)
- Run the game. The hooks will be automatically loaded.

## Developer Guide

### Prerequisites
- Windows 10 (or newer)
- Visual Studio (supporting v143 toolset, i.e. 2022+)
- Windows SDK 10.0 (supporting v143 toolset)
- MinHook (included in the project)

By default, building copies the `version.dll` to the default Steam installation directory of Burger Shop 3.
To change this, modify `CopyToGame` or `GameRoot` in the project properties (.vcxproj).  

### Building the Library
Just open the solution in Visual Studio and build the project. Keep in mind that only x86 is supported, as Burger Shop 3 is a 32-bit application.

### Using the Library
To use the library, just copy the `version.dll` from the releases (or built manually) to the game's directory. It'll automatically load the DLL when the game starts.

To build projects with the library, include the header file `bs3hooklib.h` in your project and link against the library `bs3hooklib.lib`. These files are also included in the releases.  
You can register hooks by creating a class extending `HookBase`, overriding:
- const std::string Name() - A constant string that represents the name of the hook.
- LPVOID GetHookAddress() - The address of the function you want to hook **relative to the game's base address**.
- LPVOID GetHookFunction() - A pointer function that will be called when the hooked function is called.
- LPVOID* GetOriginalFunction() - A (double) pointer to original function. Used to call the original function from the hook.
Optionally, you can override:
- Init() - Called before the hook is registered. This is where you should get pointers to variables and other functions you need.
- Hook() - Creates the hook. It's usually not necessary to override this function, but you can if you need to do something special.
- bool Command(StringNoCase) - Called when a command is entered in the console. Return true if the command was handled, false otherwise.
- void PrintCommandList() - Called when the user enters `help` in the console. Should print a list of available commands if you have any. If you don't have any commands, you don't need to override this function.
From the DLL's entry point, call `RegisterHook` with all the hooks you want to register. See the example below.
```cpp
class MyHook : public HookBase {
public:
    // The typedef for the function's signature
    typedef void(__thiscall* tMethodToOverride)(void* thisPtr, int aValue);
    // Static variable to store the original function
    static tMethodToOverride MethodToOverride;

    // Name of the hook
    const std::string Name() override {
        return "Template Hook";
    }

    // Address of the function to hook
    LPVOID GetHookAddress() override {
        return (LPVOID)0x4200;
    }

    // Function to call instead when the hooked function is called
    LPVOID GetHookFunction() override {
        return &DoStuff; // Pointer to the hook function
    }

    // Pointer to the original function
    LPVOID* GetOriginalFunction() override {
        return reinterpret_cast<LPVOID*>(&MethodToOverride);
    }

    // The hook function
    static void __fastcall DoStuff(void* thisPtr, void* _, int aValue) {
        // Do stuff, e.g.
        std::cout << "The value is " << aValue << std::endl;

        // Call original function if needed, e.g.
        MethodToOverride(thisPtr, aValue);
    }
};
MyHook::tMethodToOverride MyHook::MethodToOverride = nullptr; // Initialize the static variable

// Register the hooks
void RegisterHooks() {
    RegisterHook(new MyHook());
}

// DLL entry point
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
```

After building the project, copy the DLL to `mods` in the game's directory. The hooking library will automatically load the DLL when the game starts.

### Example Hooks
The solution contains three example hooks:
- `ExampleHook` - Creates an empty hook function on the game's update function and a "spawn" command that spawns a customer.
- `CustomCustomerHook` - Adds a custom customer to the map named "BigBurgers" using the Surfer sprite. You'll still need to create an XML file for the customer for it to work.
- `ImpatientCustomerHook` - The hook used in the guide. It makes customers very impatient.