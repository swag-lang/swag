#pragma once
#include "Attribute.h"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Should match bootstrap.swg
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum AttributeUsage
{
    // Usage
    Enum           = 0x00000001,
    EnumValue      = 0x00000002,
    StructVariable = 0x00000004,
    GlobalVariable = 0x00000008,
    Struct         = 0x00000010,
    Function       = 0x00000020,
    File           = 0x00000040,
    All            = 0x0FFFFFFF,
    // Flags
    Multi = 0x80000000,
};

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
    void* data   = nullptr;
    void* itable = nullptr;
} SwagInterface;

typedef struct SwagTempAllocator
{
    void*    data     = nullptr;
    uint64_t capacity = 0;
    uint64_t used     = 0;
    uint64_t lastUsed = 0;
    uint64_t maxUsed  = 0;
} SwagTempAllocator;

static const auto MAX_LEN_ERROR_MSG = 128;
typedef struct SwagContext
{
    SwagInterface     allocator;
    uint64_t          flags;
    SwagTempAllocator tempAllocator;
    uint8_t           errorMsg[MAX_LEN_ERROR_MSG];
    uint32_t          errorMsgLen;
    uint32_t          padding;
} SwagContext;

typedef struct SwagSlice
{
    void*    addr  = nullptr;
    uint64_t count = 0;
} SwagSlice;

typedef void (*SwagBytecodeRun)(void*, ...);
typedef void (*SwagThreadRun)(void*);
typedef void* (*SwagMakeCallback)(void*);

typedef struct SwagProcessInfos
{
    SwagSlice        arguments;
    uint64_t         contextTlsId;
    SwagContext*     defaultContext;
    SwagBytecodeRun  byteCodeRun;
    SwagMakeCallback makeCallback;
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
    // Module informations
    uint32_t moduleVersion  = 0;
    uint32_t moduleRevision = 0;
    uint32_t moduleBuildNum = 0;

    // Debug
    static const auto SAFETY_NP    = ATTRIBUTE_SAFETY_NP_ON;
    static const auto SAFETY_BC    = ATTRIBUTE_SAFETY_BC_ON;
    static const auto SAFETY_OF    = ATTRIBUTE_SAFETY_OF_ON;
    static const auto SAFETY_MT    = ATTRIBUTE_SAFETY_MT_ON;
    static const auto SAFETY_AN    = ATTRIBUTE_SAFETY_AN_ON;
    uint64_t          safetyGuards = 0xFFFFFFFF'FFFFFFFF;

    // Bytecode
    bool byteCodeOptimize = true;
    bool byteCodeInline   = true;

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

enum class TypeInfoKind : uint8_t
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
    NameAlias,
    Code,
    Interface,
    TypeSet,
    Count,
};

enum class NativeTypeKind : uint8_t
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
    UInt,
    Int,
    Count,
};

enum class TypeInfoFlags : uint16_t
{
    None        = 0x0000,
    TypeInfoPtr = 0x0001,
    Integer     = 0x0002,
    Float       = 0x0004,
    Unsigned    = 0x0008,
    HasPostCopy = 0x0010,
    HasPostMove = 0x0020,
    HasDrop     = 0x0040,
    Strict      = 0x0080,
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct ConcreteSlice
{
    void*    buffer;
    uint64_t count;
};

struct ConcreteRelativeSlice
{
    int64_t  buffer;
    uint64_t count;
};

struct ConcreteTypeInfo
{
    ConcreteSlice name;
    ConcreteSlice flatName;
    uint64_t      sizeOf;
    TypeInfoKind  kind;
    uint16_t      flags;
};

struct ConcreteAny
{
    void*             value;
    ConcreteTypeInfo* type;
};

struct ConcreteTypeInfoNative
{
    ConcreteTypeInfo base;
    NativeTypeKind   nativeKind;
};

struct ConcreteAttributeParameter
{
    ConcreteSlice name;
    ConcreteAny   value;
};

struct ConcreteAttribute
{
    ConcreteSlice         name;
    ConcreteRelativeSlice params;
};

struct ConcreteTypeInfoPointer
{
    ConcreteTypeInfo base;
    int64_t          pointedType;
};

struct ConcreteTypeInfoReference
{
    ConcreteTypeInfo base;
    int64_t          pointedType;
};

struct ConcreteTypeInfoAlias
{
    ConcreteTypeInfo base;
    int64_t          rawType;
};

struct ConcreteTypeInfoParam
{
    ConcreteSlice         name;
    int64_t               pointedType;
    void*                 value;
    ConcreteRelativeSlice attributes;
    uint32_t              offsetOf;
    uint32_t              padding;
};

struct ConcreteTypeInfoStruct
{
    ConcreteTypeInfo      base;
    ConcreteRelativeSlice generics;
    ConcreteRelativeSlice fields;
    ConcreteRelativeSlice methods;
    ConcreteRelativeSlice interfaces;
    ConcreteRelativeSlice attributes;
};

struct ConcreteTypeInfoFunc
{
    ConcreteTypeInfo      base;
    ConcreteRelativeSlice generics;
    ConcreteRelativeSlice parameters;
    int64_t               returnType;
    ConcreteRelativeSlice attributes;
};

struct ConcreteTypeInfoEnum
{
    ConcreteTypeInfo      base;
    ConcreteRelativeSlice values;
    int64_t               rawType;
    ConcreteRelativeSlice attributes;
};

struct ConcreteTypeInfoArray
{
    ConcreteTypeInfo base;
    int64_t          pointedType;
    int64_t          finalType;
    uint64_t         count;
    uint64_t         totalCount;
};

struct ConcreteTypeInfoSlice
{
    ConcreteTypeInfo base;
    int64_t          pointedType;
};

struct ConcreteTypeInfoVariadic
{
    ConcreteTypeInfo base;
    int64_t          rawType;
};

struct ConcreteTypeInfoGeneric
{
    ConcreteTypeInfo base;
    int64_t          rawType;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class CompilerMsgKind
{
    PassAfterSemantic,
    PassBeforeRunByteCode,
    PassBeforeOutput,
    PassAllDone,
    SemanticFunc,
};

enum class CompilerMsgKindMask : uint64_t
{
    PassAfterSemantic     = 1 << (uint32_t) CompilerMsgKind::PassAfterSemantic,
    PassBeforeRunByteCode = 1 << (uint32_t) CompilerMsgKind::PassBeforeRunByteCode,
    PassBeforeOutput      = 1 << (uint32_t) CompilerMsgKind::PassBeforeOutput,
    PassAllDone           = 1 << (uint32_t) CompilerMsgKind::PassAllDone,
    SemanticFunc          = 1 << (uint32_t) CompilerMsgKind::SemanticFunc,
    All                   = 0xFFFFFFFFFFFFFFFF,
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

static const uint64_t SWAG_LAMBDA_BC_MARKER      = 0x8000000000000000;
static const uint64_t SWAG_LAMBDA_FOREIGN_MARKER = 0x4000000000000000;
static const uint64_t SWAG_LAMBDA_MARKER_MASK    = 0xC000000000000000;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
namespace Runtime
{
    bool     strcmp(const void* str1, uint32_t num1, const void* str2, uint32_t num2);
    float    abs(float value);
    void     print(const void* message, uint32_t len);
    void     print(int64_t value);
    void     print(double value);
    void     panic(const void* message, uint32_t size, ConcreteCompilerSourceLocation* location);
    void     error(const void* message, uint32_t size, ConcreteCompilerSourceLocation* location);
    uint64_t tlsAlloc();
    void     tlsSetValue(uint64_t id, void* value);
    void*    tlsGetValue(uint64_t id);
    void     clearContextError();

    const uint32_t COMPARE_STRICT   = 0x00000000;
    const uint32_t COMPARE_CAST_ANY = 0x00000001;
    bool           compareType(const void* type1, const void* type2, uint32_t flags);
    void*          interfaceOf(const void* structType, const void* itfType);

} // namespace Runtime