#pragma once
#ifndef PE_H
#define PE_H

#include <Windows.h>
#include <vector>
#include "cfg.h"
#include "process.h"

namespace pe {

    inline bool load_config(HANDLE proc, ULONG_PTR base, cfg::load_config_t& out) {
        LONG e_lfanew = 0;
        if (!process::read(proc, base + 0x3C, e_lfanew))
            return false;

        ULONG_PTR nt = base + static_cast<ULONG_PTR>(e_lfanew);

        WORD magic = 0;
        if (!process::read(proc, nt + 0x18, magic) || magic != 0x020B)
            return false;

        DWORD rva = 0, sz = 0;
        ULONG_PTR lc_entry = nt + 24 + 0x70 + (10 * 8);
        if (!process::read(proc, lc_entry, rva) || !process::read(proc, lc_entry + 4, sz))
            return false;

        if (!rva || !sz)
            return false;

        return process::read(proc, base + rva, out);
    }

    inline std::vector<ULONG_PTR> cfg_targets(HANDLE proc, ULONG_PTR base) {
        std::vector<ULONG_PTR> targets;

        cfg::load_config_t lc{};
        if (!load_config(proc, base, lc))
            return targets;

        if (!(lc.guard_flags & cfg::guard_flag))
            return targets;

        if (!lc.guard_fn_table || !lc.guard_fn_count)
            return targets;

        DWORD stride = (lc.guard_flags >> 28) & 0xF;
        targets.reserve(static_cast<size_t>(lc.guard_fn_count));

        ULONG_PTR ptr = static_cast<ULONG_PTR>(lc.guard_fn_table);
        for (ULONGLONG i = 0; i < lc.guard_fn_count; i++) {
            DWORD rva = 0;
            if (!process::read(proc, ptr, rva))
                break;
            targets.push_back(base + rva);
            ptr += 4 + stride;
        }

        return targets;
    }

}

#endif