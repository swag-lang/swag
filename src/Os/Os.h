#pragma once

struct BackendTarget;
struct BuildParameters;
struct ByteCodeRunContext;
struct Module;
struct TypeInfoFuncAttr;
struct BuildCfg;
struct CpuPushParam;

namespace OS
{
    enum class Key
    {
        Left,
        Right,
        Up,
        Down,
        Tab,
        Return,
        Home,
        End,
        Delete,
        Back,
        Ascii,
        Escape,
        PasteFromClipboard,
    };

    void                 setup();
    bool                 setupBuild();
    const BackendTarget& getNativeTarget();
    void                 cls();

    bool  doProcess(Module* module, const Utf8& cmdline, const std::string& currentDirectory, uint32_t& numErrors);
    void  setThreadName(std::thread* thread, const char* threadName);
    void  exit(int code);
    void* loadLibrary(const char* name);
    void* getProcAddress(void* handle, const char* name);
    bool  isDebuggerAttached();
    void  raiseException(int code, const char* msg = nullptr);
    Utf8  captureStack();

    Utf8 getClipboardString();
    Key  promptChar(int& c, bool& ctrl, bool& shift);

    Path getExePath();
    Utf8 getLastErrorAsString();
    void errorBox(const char* title, const char* expr);
    void assertBox(const char* expr, const char* file, int line);
    bool patchIcon(const std::wstring& filename, const std::wstring& path, Utf8& error);

    void visitFiles(const char* folder, const std::function<void(const char*)>& user);
    void visitFolders(const char* folder, const std::function<void(const char*)>& user, const char* match = "*");
    void visitFilesFolders(const char* folder, const std::function<void(uint64_t, const char*, bool)>& user);
    void visitFilesRec(const char* folder, const std::function<void(const char*)>& user);

    void     ensureFileIsWritten(const char* fileName);
    uint64_t getFileWriteTime(const char* fileName);
    Path     getTemporaryFolder();

    uint64_t timerNow();
    double   timerToSeconds(uint64_t timer);

    void ffi(ByteCodeRunContext* context, void* foreignPtr, const TypeInfoFuncAttr* typeInfoFunc, const VectorNative<CpuPushParam>& pushCPUParams, void* retCopyAddr);

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
    int8_t  atomicXchg(int8_t* addr, int8_t replaceWith);
    int16_t atomicXchg(int16_t* addr, int16_t replaceWith);
    int32_t atomicXchg(int32_t* addr, int32_t replaceWith);
    int64_t atomicXchg(int64_t* addr, int64_t replaceWith);
    int8_t  atomicCmpXchg(int8_t* addr, int8_t compareTo, int8_t replaceWith);
    int16_t atomicCmpXchg(int16_t* addr, int16_t compareTo, int16_t replaceWith);
    int32_t atomicCmpXchg(int32_t* addr, int32_t compareTo, int32_t replaceWith);
    int64_t atomicCmpXchg(int64_t* addr, int64_t compareTo, int64_t replaceWith);

    uint64_t tlsAlloc();
    void     tlsSetValue(uint64_t id, void* value);
    void*    tlsGetValue(uint64_t id);

    uint8_t  bitCountNz(uint8_t value);
    uint16_t bitCountNz(uint16_t value);
    uint32_t bitCountNz(uint32_t value);
    uint64_t bitCountNz(uint64_t value);
    uint8_t  bitCountTz(uint8_t value);
    uint16_t bitCountTz(uint16_t value);
    uint32_t bitCountTz(uint32_t value);
    uint64_t bitCountTz(uint64_t value);
    uint8_t  bitCountLz(uint8_t value);
    uint16_t bitCountLz(uint16_t value);
    uint32_t bitCountLz(uint32_t value);
    uint64_t bitCountLz(uint64_t value);

    uint16_t byteSwap(uint16_t value);
    uint32_t byteSwap(uint32_t value);
    uint64_t byteSwap(uint64_t value);

#ifdef _WIN32
#define SWAG_TRY                          __try
#define SWAG_EXCEPT                       __except
#define SWAG_EXCEPTION_EXECUTE_HANDLER    EXCEPTION_EXECUTE_HANDLER
#define SWAG_EXCEPTION_CONTINUE_EXECUTION EXCEPTION_CONTINUE_EXECUTION
#define SWAG_LPEXCEPTION_POINTERS         LPEXCEPTION_POINTERS
#define SWAG_GET_EXCEPTION_INFOS()        GetExceptionInformation()
#endif
} // namespace OS
