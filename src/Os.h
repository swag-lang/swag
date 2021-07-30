#pragma once
#include "Log.h"

struct OutputFile;
struct BuildParameters;
struct Module;

namespace OS
{
    void setup();
    void setupBackend();
    void consoleSetColor(LogColor color);

    bool  doProcess(Module* module, const Utf8& cmdline, const string& currentDirectory, bool logAll, uint32_t& numErrors, LogColor logColor = LogColor::DarkCyan, const char* logPrefix = nullptr);
    void  doRunProcess(const Utf8& cmdline, const string& currentDirectory);
    void  setThreadName(thread* thread, const char* threadName);
    void  exit(int code);
    void* loadLibrary(const char* name);
    void* getProcAddress(void* handle, const char* name);

    Utf8 getLastErrorAsString();
    void errorBox(const char* expr, const char* title);
    void assertBox(const char* expr, const char* file, int line);

    void visitFiles(const char* folder, function<void(const char*)> user);
    void visitFolders(const char* folder, function<void(const char*)> user, const char* match = "*");
    void visitFilesFolders(const char* folder, function<void(uint64_t, const char*, bool)> user);
    void visitFilesRec(const char* folder, function<void(const char*)> user);

    uint64_t getFileWriteTime(const char* fileName);
    bool     getSwagFolder(string& folder);
    void     setSwagFolder(const string& folder);
    bool     touchFile(const fs::path& path);
    string   getTemporaryFolder();

    OutputFile* newOutputFile();
    void        freeOutputFile(OutputFile* file);

    uint64_t timerNow();
    double   timerToSeconds(uint64_t timer);

    bool    atomicTestNull(void** ptr);
    void    atomicSetIfNotNull(void** ptr, void* what);
    int8_t  atomicAdd(int8_t* addr, int8_t value);
    int16_t atomicAdd(int16_t* addr, int16_t value);
    int32_t atomicAdd(int32_t* addr, int32_t value);
    int64_t atomicAdd(int64_t* addr, int64_t value);
    int8_t  atomicAnd(int8_t* addr, int8_t value);
    int16_t atomicAnd(int16_t* addr, int16_t value);
    int32_t atomicAnd(int32_t* addr, int32_t value);
    int64_t atomicAnd(int64_t* addr, int64_t value);
    int8_t  atomicOr(int8_t* addr, int8_t value);
    int16_t atomicOr(int16_t* addr, int16_t value);
    int32_t atomicOr(int32_t* addr, int32_t value);
    int64_t atomicOr(int64_t* addr, int64_t value);
    int8_t  atomicXor(int8_t* addr, int8_t value);
    int16_t atomicXor(int16_t* addr, int16_t value);
    int32_t atomicXor(int32_t* addr, int32_t value);
    int64_t atomicXor(int64_t* addr, int64_t value);
    int8_t  atomicXchg(int8_t* addr, int8_t value);
    int16_t atomicXchg(int16_t* addr, int16_t value);
    int32_t atomicXchg(int32_t* addr, int32_t value);
    int64_t atomicXchg(int64_t* addr, int64_t value);
    int8_t  atomicCmpXchg(int8_t* addr, int8_t compareTo, int8_t replaceWith);
    int16_t atomicCmpXchg(int16_t* addr, int16_t compareTo, int16_t replaceWith);
    int32_t atomicCmpXchg(int32_t* addr, int32_t compareTo, int32_t replaceWith);
    int64_t atomicCmpXchg(int64_t* addr, int64_t compareTo, int64_t replaceWith);

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
