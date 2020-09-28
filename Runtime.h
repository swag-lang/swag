#pragma once

// !!!!!!!!!!!!!!!!!!!!!!!!!!!
// SHOULD MATCH EVERY BACKENDS
// !!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class ContextFlags : uint64_t
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
    uint64_t      flags;
} SwagContext;

typedef struct SwagSlice
{
    void*    addr;
    uint64_t count;
} SwagSlice;

typedef void (*SwagBytecodeRun)(void*, ...);

typedef struct SwagProcessInfos
{
    SwagSlice       arguments;
    uint64_t        contextTlsId;
    SwagContext*    defaultContext;
    SwagBytecodeRun byteCodeRun;
} SwagProcessInfos;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BuildCfgBackendLLVM
{
    uint32_t minFunctionPerFile = 256;
    uint32_t maxFunctionPerFile = 1024;
    bool     outputIR           = false; // Write a 'file.ir' text file just next to the output file
};

struct BuildCfgBackendX64
{
    uint32_t minFunctionPerFile = 256;
    uint32_t maxFunctionPerFile = 1024;
};

struct BuildCfg
{
    // Debug
    bool safetyGuards = true;

    // Bytecode
    uint32_t byteCodeMaxRecurse = 1024;
    uint32_t byteCodeStackSize  = 16 * 1024;
    uint32_t byteCodeOptimize   = 1;

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

enum class TypeInfoKind : uint16_t
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

enum class TypeInfoFlags : uint16_t
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
    void*    buffer;
    uint64_t count;
};

struct ConcreteTypeInfo
{
    ConcreteSlice name;
    uint32_t      sizeOf;
    TypeInfoKind  kind;
    uint16_t      flags;
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
    uint32_t          ptrCount;
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
    uint32_t          offsetOf;
    uint32_t          padding;
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
    uint32_t          count;
    uint32_t          totalCount;
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

enum class CompilerMsgKindMask : uint64_t
{
    PassBeforeSemantic = 1 << (uint32_t) CompilerMsgKind::PassBeforeSemantic,
    PassBeforeRun      = 1 << (uint32_t) CompilerMsgKind::PassBeforeRun,
    PassBeforeOutput   = 1 << (uint32_t) CompilerMsgKind::PassBeforeOutput,
    PassAllDone        = 1 << (uint32_t) CompilerMsgKind::PassAllDone,
    SemanticFunc       = 1 << (uint32_t) CompilerMsgKind::SemanticFunc,
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
    uint32_t      lineStart, colStart;
    uint32_t      lineEnd, colEnd;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
namespace Runtime
{
    int      memcmp(const void* b1, const void* b2, size_t n);
    bool     strcmp(const void* str1, uint32_t num1, const void* str2, uint32_t num2);
    float    abs(float value);
    void     print(const void* message, uint32_t len);
    void     print(int64_t value);
    void     print(double value);
    void     assertMsg(const void* message, uint32_t size, ConcreteCompilerSourceLocation* location);
    void     error(const void* message, uint32_t size, ConcreteCompilerSourceLocation* location);
    uint64_t tlsAlloc();
    void     tlsSetValue(uint64_t id, void* value);
    void*    tlsGetValue(uint64_t id);

    const uint32_t COMPARE_STRICT   = 0x00000000;
    const uint32_t COMPARE_CAST_ANY = 0x00000001;
    bool           compareType(const void* type1, const void* type2, uint32_t flags);
    void*          interfaceOf(const void* structType, const void* itfType);

} // namespace Runtime