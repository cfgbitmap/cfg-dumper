#include <Windows.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#include <cstdio>
#include <vector>

#include "cfg.h"
#include "process.h"
#include "pe.h"

#pragma comment(lib, "dbghelp.lib")

static bool resolve(HANDLE proc, ULONG_PTR addr, char* buf, size_t n) {
    char tmp[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
    auto* sym = reinterpret_cast<SYMBOL_INFO*>(tmp);
    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen = MAX_SYM_NAME;

    DWORD64 disp = 0;
    if (!SymFromAddr(proc, static_cast<DWORD64>(addr), &disp, sym) || disp)
        return false;

    strncpy_s(buf, n, sym->Name, _TRUNCATE);
    return true;
}

static DWORD resolve_pid(const char* arg) {
    for (const char* c = arg; *c; c++)
        if (!isdigit(static_cast<unsigned char>(*c))) {
            wchar_t w[MAX_PATH]{};
            MultiByteToWideChar(CP_ACP, 0, arg, -1, w, MAX_PATH);
            return process::pid_from_name(w);
        }
    return static_cast<DWORD>(atoi(arg));
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }

    DWORD pid = resolve_pid(argv[1]);
    if (!pid) return 1;

    HANDLE proc = process::open(pid);
    if (!proc) return 1;

    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    SymInitialize(proc, nullptr, TRUE);

    process::module_t mods[512]{};
    size_t mod_count = 0;
    process::modules(pid, mods, 512, mod_count);

    printf("pid %lu / %zu modules\n\n", pid, mod_count);

    ULONG_PTR total = 0;

    for (size_t i = 0; i < mod_count; i++) {
        const auto& mod = mods[i];
        auto targets = pe::cfg_targets(proc, mod.base);
        if (targets.empty())
            continue;

        printf("%ls  (%zu)\n", mod.name, targets.size());

        for (size_t j = 0; j < targets.size(); j++) {
            ULONG_PTR addr = targets[j];
            char sym[256]{};

            if (resolve(proc, addr, sym, sizeof(sym)))
                printf("  %016llx  %s\n", static_cast<ULONGLONG>(addr), sym);
            else
                printf("  %016llx  +0x%llx\n",
                    static_cast<ULONGLONG>(addr),
                    static_cast<ULONGLONG>(addr - mod.base));
        }

        printf("\n");
        total += targets.size();
    }

    printf("%llu targets\n", static_cast<ULONGLONG>(total));

    SymCleanup(proc);
    CloseHandle(proc);
    return 0;
}