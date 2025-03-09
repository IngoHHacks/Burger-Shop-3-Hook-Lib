#ifndef BS3HOOKLIB_H
#define BS3HOOKLIB_H

#ifdef BS3HOOKLIB_EXPORTS
#define BS3HOOKLIB_API __declspec(dllexport)
#else
#define BS3HOOKLIB_API __declspec(dllimport)
#endif

#include <string>
#include <iostream>
#include <type_traits>
#include <vector>
#include <Windows.h>
#include <algorithm>

#define LOG_INFO 0
#define LOG_WARNING 1
#define LOG_ERROR 2

inline std::string Pad(const std::string& str, const int width) {
    return str + std::string(width - str.length(), ' ');
}

inline std::string PadLeft(const std::string& str, const int width) {
    return std::string(width - str.length(), ' ') + str;
}

inline std::string FormatNameLevel(const std::string& name, const int level, const int leftWidth = 10, const int rightWidth = 10) {
    std::string levelStr;
    switch (level) {
        case LOG_INFO:
            levelStr = "INFO";
            break;
        case LOG_WARNING:
            levelStr = "WARNING";
            break;
        case LOG_ERROR:
            levelStr = "ERROR";
            break;
        default:
            levelStr = "???";
    }
    return "[" + PadLeft(name, leftWidth) + " : " + Pad(levelStr, rightWidth) + "] ";
}


inline std::string FormatNameLevel(const std::string& name, const std::string& level, const int leftWidth = 12, const int rightWidth = 8) {
    return "[" + PadLeft(name, leftWidth) + " : " + Pad(level, rightWidth) + "] ";
}

class StringNoCase : public std::string {
    public:
        StringNoCase() : std::string() {}
        StringNoCase(const std::string& str) : std::string(str) {}
        StringNoCase(const char* str) : std::string(str) {}

        bool operator==(const std::string& other) const {
            return _stricmp(c_str(), other.c_str()) == 0;
        }
        bool operator!=(const std::string& other) const {
            return _stricmp(c_str(), other.c_str()) != 0;
        }
        bool operator<(const std::string& other) const {
            return _stricmp(c_str(), other.c_str()) < 0;
        }
        bool operator>(const std::string& other) const {
            return _stricmp(c_str(), other.c_str()) > 0;
        }
        bool operator<=(const std::string& other) const {
            return _stricmp(c_str(), other.c_str()) <= 0;
        }
        bool operator>=(const std::string& other) const {
            return _stricmp(c_str(), other.c_str()) >= 0;
        }

        bool operator==(const char* other) const {
            return _stricmp(c_str(), other) == 0;
        }
        bool operator!=(const char* other) const {
            return _stricmp(c_str(), other) != 0;
        }
        bool operator<(const char* other) const {
            return _stricmp(c_str(), other) < 0;
        }
        bool operator>(const char* other) const {
            return _stricmp(c_str(), other) > 0;
        }
        bool operator<=(const char* other) const {
            return _stricmp(c_str(), other) <= 0;
        }
        bool operator>=(const char* other) const {
            return _stricmp(c_str(), other) >= 0;
        }

        bool operator==(const StringNoCase& other) const {
            return _stricmp(c_str(), other.c_str()) == 0;
        }
        bool operator!=(const StringNoCase& other) const {
            return _stricmp(c_str(), other.c_str()) != 0;
        }
        bool operator<(const StringNoCase& other) const {
            return _stricmp(c_str(), other.c_str()) < 0;
        }
        bool operator>(const StringNoCase& other) const {
            return _stricmp(c_str(), other.c_str()) > 0;
        }
        bool operator<=(const StringNoCase& other) const {
            return _stricmp(c_str(), other.c_str()) <= 0;
        }
        bool operator>=(const StringNoCase& other) const {
            return _stricmp(c_str(), other.c_str()) >= 0;
        }

        friend bool operator==(const std::string& str, const StringNoCase& other) {
            return _stricmp(str.c_str(), other.c_str()) == 0;
        }
        friend bool operator!=(const std::string& str, const StringNoCase& other) {
            return _stricmp(str.c_str(), other.c_str()) != 0;
        }
        friend bool operator<(const std::string& str, const StringNoCase& other) {
            return _stricmp(str.c_str(), other.c_str()) < 0;
        }
        friend bool operator>(const std::string& str, const StringNoCase& other) {
            return _stricmp(str.c_str(), other.c_str()) > 0;
        }
        friend bool operator<=(const std::string& str, const StringNoCase& other) {
            return _stricmp(str.c_str(), other.c_str()) <= 0;
        }
        friend bool operator>=(const std::string& str, const StringNoCase& other) {
            return _stricmp(str.c_str(), other.c_str()) >= 0;
        }

        friend bool operator==(const char* str, const StringNoCase& other) {
            return _stricmp(str, other.c_str()) == 0;
        }
        friend bool operator!=(const char* str, const StringNoCase& other) {
            return _stricmp(str, other.c_str()) != 0;
        }
        friend bool operator<(const char* str, const StringNoCase& other) {
            return _stricmp(str, other.c_str()) < 0;
        }
        friend bool operator>(const char* str, const StringNoCase& other) {
            return _stricmp(str, other.c_str()) > 0;
        }
        friend bool operator<=(const char* str, const StringNoCase& other) {
            return _stricmp(str, other.c_str()) <= 0;
        }
        friend bool operator>=(const char* str, const StringNoCase& other) {
            return _stricmp(str, other.c_str()) >= 0;
        }

        operator std::string() const {
            return *this;
        }

        operator const char*() const {
            return c_str();
        }

        friend std::ostream& operator<<(std::ostream& os, const StringNoCase& str) {
            os << str.c_str();
            return os;
        }

        friend std::istream& operator>>(std::istream& is, StringNoCase& str) {
            std::string temp;
            is >> temp;
            str = temp;
            return is;
        }

        std::string ToLower() const {
            std::string lower = *this;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return lower;
        }

        std::string ToUpper() const {
            std::string upper = *this;
            std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
            return upper;
        }

        bool StartsWith(const std::string& prefix) const {
            return size() >= prefix.size() && compare(0, prefix.size(), prefix) == 0;
        }

        bool EndsWith(const std::string& suffix) const {
            return size() >= suffix.size() && compare(size() - suffix.size(), suffix.size(), suffix) == 0;
        }

        std::string Trim() const {
            std::string str = *this;
            str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
            str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
            return str;
        }
};

class MemPtr {
    private:
        uintptr_t ptr;

    public:
        MemPtr() : ptr(0) {}
        explicit MemPtr(uintptr_t addr) : ptr(addr) {}
        MemPtr(int addr) : ptr(addr) {}

        template <typename T>
        MemPtr(T* addr) : ptr(reinterpret_cast<uintptr_t>(addr)) {}

        template <typename T>
        operator T() {
            if constexpr (std::is_pointer_v<T>) {
                return reinterpret_cast<T>(ptr);
            } else if constexpr (std::is_integral_v<T>) {
                return static_cast<T>(ptr);
            } else {
                static_assert(sizeof(T) == 0, "Unsupported conversion type");
            }
        }

        MemPtr operator+(const MemPtr other) const {
            return ptr + other.ptr;
        }

        MemPtr operator-(const MemPtr other) const {
            return ptr - other.ptr;
        }

        template <typename T>
        MemPtr& operator+=(const T offset) {
            ptr += offset;
            return *this;
        }

        template <typename T>
        MemPtr& operator-=(const T offset) {
            ptr -= offset;
            return *this;
        }

        template <typename T>
        friend MemPtr operator+(const T offset, const MemPtr& ptr) {
            return ptr + offset;
        }

        template <typename T>
        friend MemPtr operator-(const T offset, const MemPtr& ptr) {
            return ptr - offset;
        }
       
        template <typename T>
        bool operator==(const T other) const {
            return ptr == other;
        }

        template <typename T>
        bool operator!=(const T other) const {
            return ptr != other;
        }

        uintptr_t Get() const {
            return ptr;
        }

        int AsInt() const {
            return static_cast<int>(ptr);
        }

        LPVOID AsPtr() const {
            return reinterpret_cast<LPVOID>(ptr);
        }
};

class BS3HOOKLIB_API HookBase {
protected:
    virtual LPVOID GetHookAddress() = 0;
    virtual LPVOID GetHookFunction() = 0;
    virtual LPVOID* GetOriginalFunction() = 0;
public:
    virtual const std::string Name() = 0;
    virtual void Init();
    virtual void Hook();
    virtual bool Command(const StringNoCase& command);
    virtual void PrintCommandHelp();
    void Log(const std::string& message, const int level = LOG_INFO);
    void LogInfo(const std::string& message);
    void LogWarning(const std::string& message);
    void LogError(const std::string& message);
};

BS3HOOKLIB_API void RegisterHook(HookBase *hook);

BS3HOOKLIB_API HANDLE GetProcess();
BS3HOOKLIB_API DWORD GetPID();
BS3HOOKLIB_API MemPtr GetBaseAddress();

BS3HOOKLIB_API const MemPtr GetMyAppPointer();
BS3HOOKLIB_API const MemPtr GetMyAppBoardOffset();

inline MemPtr ReadPointer(HANDLE hProcess, MemPtr address) {
    uintptr_t value;
    ReadProcessMemory(hProcess, address, &value, sizeof(value), nullptr);
    return value;
}

template <typename T>
inline T ReadMemory(HANDLE hProcess, MemPtr address) {
	T value{};
	if (ReadProcessMemory(hProcess, address, &value, sizeof(T), nullptr)) {
		return value;
	}
	return T();
}

inline MemPtr ReadSequenceAddress(HANDLE hProcess, const std::vector<MemPtr>& offsets) {
	if (offsets.empty()) {
		return GetBaseAddress();
	}
	uintptr_t start = GetBaseAddress() + offsets[0];
	for (size_t i = 1; i < offsets.size(); i++) { 
		start = ReadPointer(hProcess, start) + offsets[i];
	}
	return start;
}

template <typename T>
inline T ReadSequenceMemory(HANDLE hProcess, const std::vector<MemPtr>& offsets) {
	return ReadMemory<T>(hProcess, ReadSequenceAddress(hProcess, offsets));
}

#endif