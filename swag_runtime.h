#pragma once
#include "libc/stdint.h"

static const SwagU32 COMPARE_STRICT   = 0x00000000;
static const SwagU32 COMPARE_CAST_ANY = 0x00000001;

extern "C" void             swag_runtime_print(const char* msg);
extern "C" char*            swag_runtime_itoa(char* result, SwagS64 value);
extern "C" void             swag_runtime_print_n(const void* message, SwagS32 len);
extern "C" void             swag_runtime_print_i64(SwagS64 value);
extern "C" void*            swag_runtime_loadDynamicLibrary(const void* name);
extern "C" SwagU64          swag_runtime_tlsAlloc();
extern "C" void             swag_runtime_tlsSetValue(SwagU64 id, void* value);
extern "C" void*            swag_runtime_tlsGetValue(SwagU64 id);
extern "C" void             swag_runtime_convertArgcArgv(void* dest, SwagS32 argc, void* argv[]);
extern "C" void             swag_runtime_assert(bool expr, const void* file, SwagU64 colline, const void* msg);
extern "C" void             swag_runtime_assert_msg(struct ConcreteCompilerSourceLocation* location, const void* message);
extern "C" void*            swag_runtime_memcpy(void* destination, const void* source, size_t size);
extern "C" void             swag_runtime_error(struct ConcreteCompilerSourceLocation* location, const void* message, SwagU32 size);
extern "C" void             swag_runtime_setProcessInfos(struct SwagProcessInfos* infos);
extern "C" SwagProcessInfos g_SwagProcessInfos;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!
// SHOULD MATCH EVERY BACKENDS
// !!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class ContextFlags : SwagU64
{
    Test     = 0x00000000'00000001,
    DevMode  = 0x00000000'00000002,
    ByteCode = 0x00000000'00000004,
};

typedef struct SwagInterface
{
    void* data;
    void* itable;
} SwagInterface;

typedef struct SwagContext
{
    SwagInterface allocator;
    SwagU64       flags;
} SwagContext;

typedef struct SwagSlice
{
    void*   addr;
    SwagU64 count;
} SwagSlice;

typedef void (*SwagBytecodeRun)(void*, ...);

typedef struct SwagProcessInfos
{
    SwagSlice       arguments;
    SwagU64         contextTlsId;
    SwagContext*    defaultContext;
    SwagBytecodeRun byteCodeRun;
} SwagProcessInfos;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BuildCfgBackendLLVM
{
    SwagU32 minFunctionPerFile = 256;
    SwagU32 maxFunctionPerFile = 1024;
    bool    outputIR           = false; // Write a 'file.ir' text file just next to the output file
};

struct BuildCfgBackendX64
{
    SwagU32 minFunctionPerFile = 256;
    SwagU32 maxFunctionPerFile = 1024;
};

struct BuildCfg
{
    // Debug
    bool safetyGuards = true;

    // Bytecode
    SwagU32 byteCodeMaxRecurse = 1024;
    SwagU32 byteCodeStackSize  = 16 * 1024;
    SwagU32 byteCodeOptimize   = 1;

    // Backend common
    bool backendDebugInformations = false;
    bool backendOptimizeSpeed     = false;
    bool backendOptimizeSize      = false;

    // Specific backend parameters
    BuildCfgBackendLLVM backendLLVM;
    BuildCfgBackendX64  backendX64;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class TypeInfoKind : SwagU16
{
    Invalid,
    Native,
    Namespace,
    Enum,
    FuncAttr,
    Param,
    Lambda,
    Pointer,
    Reference,
    Array,
    Slice,
    TypeListTuple,
    TypeListArray,
    Variadic,
    TypedVariadic,
    Struct,
    Generic,
    Alias,
    Code,
    Interface,
    Count,
};

enum class NativeTypeKind
{
    Void,
    S8,
    S16,
    S32,
    S64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    Bool,
    Char,
    String,
    Any,
    Undefined,
    Count,
};

enum class TypeInfoFlags : SwagU16
{
    None        = 0x0000,
    TypeInfoPtr = 0x0001,
    Integer     = 0x0002,
    Float       = 0x0004,
    Unsigned    = 0x0008,
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct ConcreteSlice
{
    void*   buffer;
    SwagU64 count;
};

struct ConcreteTypeInfo
{
    ConcreteSlice name;
    SwagU32       sizeOf;
    TypeInfoKind  kind;
    SwagU16       flags;
};

struct ConcreteAny
{
    void*             value;
    ConcreteTypeInfo* type;
};

struct ConcreteAttributeParameter
{
    ConcreteSlice name;
    ConcreteAny   value;
};

struct ConcreteAttribute
{
    ConcreteSlice name;
    ConcreteSlice params;
};

struct ConcreteTypeInfoNative
{
    ConcreteTypeInfo base;
    NativeTypeKind   nativeKind;
};

struct ConcreteTypeInfoPointer
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* finalType;
    ConcreteTypeInfo* pointedType;
    SwagU32           ptrCount;
};

struct ConcreteTypeInfoReference
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

struct ConcreteTypeInfoParam
{
    ConcreteSlice     name;
    ConcreteTypeInfo* pointedType;
    void*             value;
    ConcreteSlice     attributes;
    SwagU32           offsetOf;
    SwagU32           padding;
};

struct ConcreteTypeInfoStruct
{
    ConcreteTypeInfo base;
    ConcreteSlice    fields;
    ConcreteSlice    methods;
    ConcreteSlice    interfaces;
    ConcreteSlice    attributes;
};

struct ConcreteTypeInfoList
{
    ConcreteTypeInfo base;
    ConcreteSlice    types;
};

struct ConcreteTypeInfoFunc
{
    ConcreteTypeInfo  base;
    ConcreteSlice     parameters;
    ConcreteTypeInfo* rawType;
    ConcreteTypeInfo* returnType;
    ConcreteSlice     attributes;
};

struct ConcreteTypeInfoEnum
{
    ConcreteTypeInfo  base;
    ConcreteSlice     values;
    ConcreteTypeInfo* rawType;
    ConcreteSlice     attributes;
};

struct ConcreteTypeInfoArray
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
    ConcreteTypeInfo* finalType;
    SwagU32           count;
    SwagU32           totalCount;
};

struct ConcreteTypeInfoSlice
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class CompilerMsgKind
{
    PassBeforeSemantic,
    PassBeforeRun,
    PassBeforeOutput,
    PassAllDone,
    SemanticFunc,
};

enum class CompilerMsgKindMask : SwagU64
{
    PassBeforeSemantic = 1 << (SwagU32) CompilerMsgKind::PassBeforeSemantic,
    PassBeforeRun      = 1 << (SwagU32) CompilerMsgKind::PassBeforeRun,
    PassBeforeOutput   = 1 << (SwagU32) CompilerMsgKind::PassBeforeOutput,
    PassAllDone        = 1 << (SwagU32) CompilerMsgKind::PassAllDone,
    SemanticFunc       = 1 << (SwagU32) CompilerMsgKind::SemanticFunc,
    All                = 0xFFFFFFFFFFFFFFFF,
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct ConcreteCompilerMessage
{
    ConcreteSlice     moduleName;
    CompilerMsgKind   kind;
    ConcreteSlice     name;
    ConcreteTypeInfo* type;
};

struct ConcreteCompilerSourceLocation
{
    ConcreteSlice fileName;
    SwagU32       lineStart, colStart;
    SwagU32       lineEnd, colEnd;
};
