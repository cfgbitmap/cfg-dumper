#pragma once
#ifndef CFG_H
#define CFG_H

#include <Windows.h>
#include <cstdint>

namespace cfg {

    constexpr ULONG_PTR slot_size = 16;
    constexpr ULONG_PTR slot_shift = 4;
    constexpr DWORD     guard_flag = 0x00000100;

    struct load_config_t {
        DWORD     size;
        DWORD     timestamp;
        WORD      major_ver;
        WORD      minor_ver;
        DWORD     flags_clear;
        DWORD     flags_set;
        DWORD     cs_timeout;
        ULONGLONG decommit_free;
        ULONGLONG decommit_total;
        ULONGLONG lock_prefix_table;
        ULONGLONG max_alloc_size;
        ULONGLONG vm_threshold;
        ULONGLONG process_affinity;
        DWORD     heap_flags;
        WORD      csd_version;
        WORD      dependent_load_flags;
        ULONGLONG edit_list;
        ULONGLONG security_cookie;
        ULONGLONG se_handler_table;
        ULONGLONG se_handler_count;
        ULONGLONG guard_check_fn;
        ULONGLONG guard_dispatch_fn;
        ULONGLONG guard_fn_table;
        ULONGLONG guard_fn_count;
        DWORD     guard_flags;
    };

    struct target_t {
        ULONG_PTR address;
        ULONG_PTR rva;
        char      symbol[256];
    };

}

#endif