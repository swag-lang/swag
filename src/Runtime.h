#pragma once
#include "Attribute.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

static const uint64_t ATTRIBUTE_SAFETY_NULLPTR_ON     = 0x0000100000000000;
static const uint64_t ATTRIBUTE_SAFETY_BOUNDCHECK_ON  = 0x0000200000000000;
static const uint64_t ATTRIBUTE_SAFETY_OVERFLOW_ON    = 0x0000400000000000;
static const uint64_t ATTRIBUTE_SAFETY_MATH_ON        = 0x0000800000000000;
static const uint64_t ATTRIBUTE_SAFETY_CASTANY_ON     = 0x0001000000000000;
static const uint64_t ATTRIBUTE_SAFETY_NULLPTR_OFF    = 0x0010000000000000;
static const uint64_t ATTRIBUTE_SAFETY_BOUNDCHECK_OFF = 0x0020000000000000;
static const uint64_t ATTRIBUTE_SAFETY_OVERFLOW_OFF   = 0x0040000000000000;
static const uint64_t ATTRIBUTE_SAFETY_MATH_OFF       = 0x0080000000000000;
static const uint64_t ATTRIBUTE_SAFETY_CASTANY_OFF    = 0x0100000000000000;
static const uint64_t ATTRIBUTE_SAFETY_MASK_ON        = ATTRIBUTE_SAFETY_NULLPTR_ON | ATTRIBUTE_SAFETY_BOUNDCHECK_ON | ATTRIBUTE_SAFETY_OVERFLOW_ON | ATTRIBUTE_SAFETY_MATH_ON | ATTRIBUTE_SAFETY_CASTANY_ON;
static const uint64_t ATTRIBUTE_SAFETY_MASK_OFF       = ATTRIBUTE_SAFETY_NULLPTR_OFF | ATTRIBUTE_SAFETY_BOUNDCHECK_OFF | ATTRIBUTE_SAFETY_OVERFLOW_OFF | ATTRIBUTE_SAFETY_MATH_OFF | ATTRIBUTE_SAFETY_CASTANY_OFF;
static const uint64_t ATTRIBUTE_SAFETY_MASK           = ATTRIBUTE_SAFETY_MASK_ON | ATTRIBUTE_SAFETY_MASK_OFF;

enum AttributeUsage
{
    // Usage
    Enum           = 0x00000001,
    EnumValue      = 0x00000002,
    StructVariable = 0x00000004,
    GlobalVariable = 0x00000008,
    Variable       = 0x00000010,
    Struct         = 0x00000020,
    Function       = 0x00000040,
    File           = 0x00000080,
    Constant       = 0x00000100,
    All            = 0x0FFFFFFF,
    // Flags
    Multi = 0x80000000,
};

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

typedef struct SwagScratchAllocator
{
    SwagInterface allocator;
    void*         data      = nullptr;
    uint64_t      capacity  = 0;
    uint64_t      used      = 0;
    uint64_t      lastUsed  = 0;
    uint64_t      maxUsed   = 0;
    void*         firstLeak = nullptr;
    uint64_t      totalLeak = 0;
    uint64_t      maxLeak   = 0;
} SwagTempAllocator;

typedef struct SwagSlice
{
    void*    buffer = nullptr;
    uint64_t count  = 0;
} SwagSlice;

struct SwagCompilerSourceLocation
{
    SwagSlice fileName;
    uint32_t  lineStart, colStart;
    uint32_t  lineEnd, colEnd;
};

static const auto MAX_LEN_ERROR_MSG = 128;
static const auto MAX_TRACE         = 32;
typedef struct SwagContext
{
    SwagInterface               allocator;
    uint64_t                    flags;
    SwagScratchAllocator        tempAllocator;
    uint8_t                     errorMsg[MAX_LEN_ERROR_MSG];
    uint32_t                    errorMsgStart;
    uint32_t                    errorMsgLen;
    uint32_t                    traceIndex;
    SwagCompilerSourceLocation* trace[MAX_TRACE];
    SwagCompilerSourceLocation  exceptionLoc;
    void*                       exceptionParams[3];
    void*                       panic;
} SwagContext;

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

struct BuildCfgBackendLLVM
{
    uint32_t minFunctionPerFile = 128;
    uint32_t maxFunctionPerFile = 1024;
    bool     outputIR           = false; // Write a 'file.ir' text file just next to the output file
};

struct BuildCfgBackendX64
{
    uint32_t minFunctionPerFile = 128;
    uint32_t maxFunctionPerFile = 1024;
};

enum class BuildCfgBackendKind
{
    None,
    Export,
    Executable,
    DynamicLib,
    StaticLib,
};

struct BuildCfg
{
    // Module informations
    uint32_t  moduleVersion  = 0;
    uint32_t  moduleRevision = 0;
    uint32_t  moduleBuildNum = 0;
    SwagSlice moduleNamespace;

    // Debug
    static const auto SAFETY_NP    = ATTRIBUTE_SAFETY_NULLPTR_ON;
    static const auto SAFETY_BC    = ATTRIBUTE_SAFETY_BOUNDCHECK_ON;
    static const auto SAFETY_OF    = ATTRIBUTE_SAFETY_OVERFLOW_ON;
    static const auto SAFETY_MT    = ATTRIBUTE_SAFETY_MATH_ON;
    static const auto SAFETY_AN    = ATTRIBUTE_SAFETY_CASTANY_ON;
    uint64_t          safetyGuards = 0xFFFFFFFF'FFFFFFFF;
    bool              stackTrace   = true;

    // Bytecode
    bool byteCodeOptimize    = true;
    bool byteCodeDebugInline = true;
    bool byteCodeEmitAssume  = true;
    bool byteCodeInline      = true;

    // Backend common
    BuildCfgBackendKind backendKind              = BuildCfgBackendKind::Executable;
    bool                backendDebugInformations = false;
    bool                backendOptimizeSpeed     = false;
    bool                backendOptimizeSize      = false;

    // Specific backend parameters
    BuildCfgBackendLLVM backendLLVM;
    BuildCfgBackendX64  backendX64;
};

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
    Rune,
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
    CanCopy     = 0x0100,
    Tuple       = 0x0200,
};

struct ConcreteTypeInfo
{
    SwagSlice    name;
    SwagSlice    flatName;
    uint64_t     sizeOf;
    uint16_t     flags;
    TypeInfoKind kind;
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
    SwagSlice   name;
    ConcreteAny value;
};

struct ConcreteAttribute
{
    SwagSlice name;
    SwagSlice params;
};

struct ConcreteTypeInfoPointer
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

struct ConcreteTypeInfoReference
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

struct ConcreteTypeInfoAlias
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* rawType;
};

struct ConcreteTypeInfoParam
{
    SwagSlice         name;
    ConcreteTypeInfo* pointedType;
    void*             value;
    SwagSlice         attributes;
    uint32_t          offsetOf;
    uint32_t          padding;
};

struct ConcreteTypeInfoStruct
{
    ConcreteTypeInfo base;
    void*            opInit;
    void*            opDrop;
    void*            opPostCopy;
    void*            opPostMove;
    SwagSlice        structName;
    SwagSlice        generics;
    SwagSlice        fields;
    SwagSlice        methods;
    SwagSlice        interfaces;
    SwagSlice        attributes;
};

struct ConcreteTypeInfoFunc
{
    ConcreteTypeInfo  base;
    SwagSlice         generics;
    SwagSlice         parameters;
    ConcreteTypeInfo* returnType;
    SwagSlice         attributes;
};

struct ConcreteTypeInfoEnum
{
    ConcreteTypeInfo  base;
    SwagSlice         values;
    ConcreteTypeInfo* rawType;
    SwagSlice         attributes;
};

struct ConcreteTypeInfoArray
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
    ConcreteTypeInfo* finalType;
    uint64_t          count;
    uint64_t          totalCount;
};

struct ConcreteTypeInfoSlice
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

struct ConcreteTypeInfoVariadic
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* rawType;
};

struct ConcreteTypeInfoGeneric
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* rawType;
};

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

struct ConcreteCompilerMessage
{
    SwagSlice         moduleName;
    CompilerMsgKind   kind;
    SwagSlice         name;
    ConcreteTypeInfo* type;
};

static const uint64_t SWAG_LAMBDA_BC_MARKER      = 0x8000000000000000;
static const uint64_t SWAG_LAMBDA_FOREIGN_MARKER = 0x4000000000000000;
static const uint64_t SWAG_LAMBDA_MARKER_MASK    = 0xC000000000000000;

const uint32_t SWAG_COMPARE_STRICT   = 0x00000000;
const uint32_t SWAG_COMPARE_CAST_ANY = 0x00000001;
