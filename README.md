# cfg-dumper

finds valid cfg indirect calls within a running process

---

Finds valid CFG call targets from loaded modules and prints their addresses and symbol names when available.

## requirements

- windows x64
- visual studio + windows sdk
- admin rights

## build

open `cfg-dumper.sln`, build Release x64.

add `_NT_SYMBOL_PATH` for symbol resolution on system dlls:
```
set _NT_SYMBOL_PATH=srv*C:\Symbols*https://msdl.microsoft.com/download/symbols
```

## usage

```
cfg-dumper.exe <name.exe | pid>
```

```
cfg-dumper.exe notepad.exe
cfg-dumper.exe 1234
```

## output

```
pid 14832 / 47 modules

usermgrcli.dll  (89)
  00007ff8662b1180  UMgrQueryUserContext
  00007ff8662b18b0  UMgrQueryUserToken
  00007ff8662b1f20  UMgrGetConstrainedUserToken
  00007ff8662b2380  UMgrQueryUserContextFromSid
  00007ff8662b1010  +0x1010
  ...

266747 targets
```
