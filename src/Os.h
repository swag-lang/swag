#pragma once
#include "Log.h"

struct BuildParameters;
struct Module;

namespace OS
{
    extern void setup();
    extern void setupBackend();
    extern void consoleSetColor(LogColor color);

    extern bool  doProcess(Module* module, const Utf8& cmdline, const string& currentDirectory, bool logAll, uint32_t& numErrors, LogColor logColor = LogColor::DarkCyan, const char* logPrefix = nullptr);
    extern void  doRunProcess(const Utf8& cmdline, const string& currentDirectory);
    extern void  setThreadName(thread* thread, const char* threadName);
    extern void  exit(int code);
    extern void* loadLibrary(const char* name);
    extern void* getProcAddress(void* handle, const char* name);

    extern Utf8 getLastErrorAsString();
    extern void errorBox(const char* expr, const char* title);
    extern void assertBox(const char* expr, const char* file, int line);

    extern void visitFiles(const char* folder, function<void(const char*)> user);
    extern void visitFolders(const char* folder, function<void(const char*)> user, const char* match = "*");
    extern void visitFilesFolders(const char* folder, function<void(uint64_t, const char*, bool)> user);
    extern void visitFilesRec(const char* folder, function<void(const char*)> user);

    extern uint64_t getFileWriteTime(const char* fileName);
    extern bool     getSwagFolder(string& folder);
    extern void     setSwagFolder(const string& folder);
    extern bool     touchFile(const fs::path& path);
    extern string   getTemporaryFolder();

    extern uint64_t timerNow();
    extern double   timerToSeconds(uint64_t timer);

    extern bool    atomicTestNull(void** ptr);
    extern void    atomicSetIfNotNull(void** ptr, void* what);
    extern int8_t  atomicAdd(int8_t* addr, int8_t value);
    extern int16_t atomicAdd(int16_t* addr, int16_t value);
    extern int32_t atomicAdd(int32_t* addr, int32_t value);
    extern int64_t atomicAdd(int64_t* addr, int64_t value);
    extern int8_t  atomicAnd(int8_t* addr, int8_t value);
    extern int16_t atomicAnd(int16_t* addr, int16_t value);
    extern int32_t atomicAnd(int32_t* addr, int32_t value);
    extern int64_t atomicAnd(int64_t* addr, int64_t value);
    extern int8_t  atomicOr(int8_t* addr, int8_t value);
    extern int16_t atomicOr(int16_t* addr, int16_t value);
    extern int32_t atomicOr(int32_t* addr, int32_t value);
    extern int64_t atomicOr(int64_t* addr, int64_t value);
    extern int8_t  atomicXor(int8_t* addr, int8_t value);
    extern int16_t atomicXor(int16_t* addr, int16_t value);
    extern int32_t atomicXor(int32_t* addr, int32_t value);
    extern int64_t atomicXor(int64_t* addr, int64_t value);
    extern int8_t  atomicXchg(int8_t* addr, int8_t value);
    extern int16_t atomicXchg(int16_t* addr, int16_t value);
    extern int32_t atomicXchg(int32_t* addr, int32_t value);
    extern int64_t atomicXchg(int64_t* addr, int64_t value);
    extern int8_t  atomicCmpXchg(int8_t* addr, int8_t compareTo, int8_t replaceWith);
    extern int16_t atomicCmpXchg(int16_t* addr, int16_t compareTo, int16_t replaceWith);
    extern int32_t atomicCmpXchg(int32_t* addr, int32_t compareTo, int32_t replaceWith);
    extern int64_t atomicCmpXchg(int64_t* addr, int64_t compareTo, int64_t replaceWith);

    uint64_t tlsAlloc();
    void     tlsSetValue(uint64_t id, void* value);
    void*    tlsGetValue(uint64_t id);

    uint8_t  bitcountnz(uint8_t value);
    uint16_t bitcountnz(uint16_t value);
    uint32_t bitcountnz(uint32_t value);
    uint64_t bitcountnz(uint64_t value);
    uint8_t  bitcounttz(uint8_t value);
    uint16_t bitcounttz(uint16_t value);
    uint32_t bitcounttz(uint32_t value);
    uint64_t bitcounttz(uint64_t value);
    uint8_t  bitcountlz(uint8_t value);
    uint16_t bitcountlz(uint16_t value);
    uint32_t bitcountlz(uint32_t value);
    uint64_t bitcountlz(uint64_t value);

    uint16_t byteswap(uint16_t value);
    uint32_t byteswap(uint32_t value);
    uint64_t byteswap(uint64_t value);

} // namespace OS
