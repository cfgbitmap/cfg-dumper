#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <Windows.h>
#include <TlHelp32.h>

namespace process {

    inline DWORD pid_from_name(const wchar_t* name) {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE)
            return 0;

        PROCESSENTRY32W e{ sizeof(e) };
        DWORD pid = 0;

        if (Process32FirstW(snap, &e)) {
            do {
                if (!_wcsicmp(e.szExeFile, name)) {
                    pid = e.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snap, &e));
        }

        CloseHandle(snap);
        return pid;
    }

    inline HANDLE open(DWORD pid) {
        return OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    }

    inline bool read(HANDLE proc, ULONG_PTR addr, void* out, SIZE_T n) {
        SIZE_T got = 0;
        return ReadProcessMemory(proc, reinterpret_cast<LPCVOID>(addr), out, n, &got) && got == n;
    }

    template <typename T>
    inline bool read(HANDLE proc, ULONG_PTR addr, T& out) {
        return read(proc, addr, &out, sizeof(T));
    }

    struct module_t {
        ULONG_PTR base;
        wchar_t   name[MAX_PATH];
    };

    inline bool modules(DWORD pid, module_t* out, size_t cap, size_t& count) {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snap == INVALID_HANDLE_VALUE)
            return false;

        MODULEENTRY32W e{ sizeof(e) };
        count = 0;

        if (Module32FirstW(snap, &e)) {
            do {
                if (count < cap) {
                    out[count].base = reinterpret_cast<ULONG_PTR>(e.modBaseAddr);
                    wcsncpy_s(out[count].name, e.szModule, _TRUNCATE);
                    count++;
                }
            } while (Module32NextW(snap, &e));
        }

        CloseHandle(snap);
        return count > 0;
    }

}

#endif