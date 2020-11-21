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
    extern string   getOutputFileExtension(BackendOutputType type);
    extern string   getDllFileExtension();
    extern string   getLastErrorAsString();
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

    extern bool    atomicTestNull(void** ptr);
    extern void    atomicSetIfNotNull(void** ptr, void* what);
    extern int8_t  atomicAdd(int8_t* addr, int8_t value);
    extern int16_t atomicAdd(int16_t* addr, int16_t value);
    extern int32_t atomicAdd(int32_t* addr, int32_t value);
    extern int64_t atomicAdd(int64_t* addr, int64_t value);
} // namespace OS
