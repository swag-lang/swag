#pragma once
enum class LogColor;
enum class BackendOutputType;
#include "Log.h"

struct BuildParameters;
struct Module;

namespace OS
{
    extern void     setup();
    extern void     consoleSetup();
    extern void     consoleSetColor(LogColor color);
    extern bool     doProcess(const Utf8& cmdline, const string& currentDirectory, bool logAll, uint32_t& numErrors, LogColor logColor = LogColor::DarkCyan, const char* logPrefix = nullptr);
    extern void     doRunProcess(const Utf8& cmdline, const string& currentDirectory);
    extern string   getOutputFileExtension(BackendOutputType type);
    extern string   getDllFileExtension();
    extern Utf8     getLastErrorAsString();
    extern uint64_t tlsAlloc();
    extern void     tlsSetValue(uint64_t id, void* value);
    extern void*    tlsGetValue(uint64_t id);
    extern void*    loadLibrary(const char* name);
    extern void*    getProcAddress(void* handle, const char* name);
    extern void     visitFiles(const char* folder, function<void(const char*)> user);
    extern void     visitFolders(const char* folder, function<void(const char*)> user);
    extern void     visitFilesFolders(const char* folder, function<void(uint64_t, const char*, bool)> user);
    extern void     setThreadName(thread* thread, const char* threadName);
    extern uint64_t getFileWriteTime(const char* fileName);
    extern void     errorBox(const char* expr, const char* title);
    extern void     assertBox(const char* expr, const char* file, int line);
    extern bool     getSwagFolder(string& folder);
    extern void     setSwagFolder(const string& folder);
    extern bool     touchFile(const fs::path& path);
    extern bool     watch(function<void(const string&)> cb);
    extern string   getTemporaryFolder();
    extern void     setupBackend();
    extern bool     compile(const BuildParameters& buildParameters, Module* module, const vector<string>& cFiles);
    extern bool     link(const BuildParameters& buildParameters, Module* module, vector<string>& objectFiles);
    extern void*    alloc(size_t size);
    extern void*    realloc(void* ptr, size_t size);
    extern void     free(void* ptr);
    extern uint64_t timerNow();
    extern double   timerToSeconds(uint64_t timer);
    extern int32_t  memcmp(const void* dst, const void* src, size_t size);

    extern bool atomicTestNull(void** ptr);
    extern void atomicSetIfNotNull(void** ptr, void* what);

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
} // namespace OS
