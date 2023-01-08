#pragma once
#include "Attribute.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

static const uint64_t SAFETY_BOUNDCHECK = 0x0001;
static const uint64_t SAFETY_OVERFLOW   = 0x0002;
static const uint64_t SAFETY_MATH       = 0x0004;
static const uint64_t SAFETY_ANY        = 0x0008;
static const uint64_t SAFETY_SWITCH     = 0x0010;
static const uint64_t SAFETY_BOOL       = 0x0020;
static const uint64_t SAFETY_NAN        = 0x0040;
static const uint64_t SAFETY_INTRINSICS = 0x0080;
static const uint64_t SAFETY_ALL        = 0xFFFF;

enum WarnLevel : uint8_t
{
    Enable,
    Disable,
    Error,
};

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
    // Flags
    Multi = 0x01000000,
    Gen   = 0x02000000,
    All   = 0x04000000,
};

enum class ContextFlags : uint64_t
{
    Test     = 0x00000000'00000001,
    ByteCode = 0x00000000'00000002,
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

struct SwagCVaList
{
    uint8_t buf[2048];
};

struct SwagModule
{
    SwagSlice name;
    SwagSlice types;
};

struct SwagGlobalVarToDrop
{
    void* ptr;
    void* opDrop;
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

enum class SwagBackendGenType : uint32_t
{
    ByteCode,
    X64,
    LLVM
};

enum class SwagTargetArch : uint32_t
{
    X86_64,
};

enum class SwagTargetOs : uint32_t
{
    Windows,
    Linux,
    MacOSX,
};

enum class SwagRuntimeFlags : uint64_t
{
    Zero         = 0x00000000'00000000,
    FromCompiler = 0x00000000'00000001,
};

typedef struct SwagProcessInfos
{
    SwagSlice          modules;
    SwagSlice          args;
    uint64_t           contextTlsId;
    SwagContext*       defaultContext;
    SwagBytecodeRun    byteCodeRun;
    SwagMakeCallback   makeCallback;
    SwagBackendGenType backendKind;
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
    bool      embbedImports = false;

    // Debug
    uint32_t scratchAllocatorCapacity   = 4 * 1024 * 1024;
    uint16_t safetyGuards               = SAFETY_ALL;
    bool     debugAllocator             = true;
    bool     debugAllocatorCaptureStack = true;
    bool     debugAllocatorLeaks        = true;
    bool     stackTrace                 = true;

    // Warnings
    SwagSlice warnAsErrors;
    SwagSlice warnAsWarnings;
    SwagSlice warnAsDisabled;
    bool      warnDefaultDisabled = false;
    bool      warnDefaultErrors   = false;

    // Bytecode
    uint32_t byteCodeOptimizeLevel = 1;
    bool     byteCodeDebugInline   = true;
    bool     byteCodeEmitAssume    = true;
    bool     byteCodeInline        = true;

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
    LambdaClosure,
    Pointer,
    Array,
    Slice,
    TypeListTuple,
    TypeListArray,
    Variadic,
    TypedVariadic,
    CVariadic,
    Struct,
    Generic,
    Alias,
    Code,
    Interface,
    Attribute, // Only for export
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
    CString,
    Undefined,
    UInt,
    Int,
    Count,
};

enum class TypeInfoFlags : uint32_t
{
    None              = 0x00000000,
    PointerTypeInfo   = 0x00000001,
    Integer           = 0x00000002,
    Float             = 0x00000004,
    Unsigned          = 0x00000008,
    HasPostCopy       = 0x00000010,
    HasPostMove       = 0x00000020,
    HasDrop           = 0x00000040,
    Strict            = 0x00000080,
    CanCopy           = 0x00000100,
    Tuple             = 0x00000200,
    CString           = 0x00000400,
    Generic           = 0x00000800,
    PointerRef        = 0x00001000,
    PointerArithmetic = 0x00002000,
};

struct ConcreteTypeInfo
{
    SwagSlice    fullName;
    SwagSlice    name;
    uint64_t     sizeOf;
    uint32_t     crc32;
    uint32_t     flags;
    TypeInfoKind kind;
    uint8_t      padding[3];
};

struct ConcreteAny
{
    void*             value;
    ConcreteTypeInfo* type;
};

struct ConcreteAttributeParameter
{
    SwagSlice   name;
    ConcreteAny value;
};

struct ConcreteAttribute
{
    ConcreteTypeInfo* type;
    SwagSlice         params;
};

struct ConcreteTypeValue
{
    SwagSlice         name;
    ConcreteTypeInfo* pointedType;
    void*             value;
    SwagSlice         attributes;
    uint32_t          offsetOf;
    uint32_t          crc32;
};

struct ConcreteTypeInfoNative
{
    ConcreteTypeInfo base;
    NativeTypeKind   nativeKind;
};

struct ConcreteTypeInfoPointer
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

struct ConcreteTypeInfoAlias
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* rawType;
};

struct ConcreteTypeInfoNamespace
{
    ConcreteTypeInfo base;
};

struct ConcreteTypeInfoStruct
{
    ConcreteTypeInfo  base;
    void*             opInit;
    void*             opDrop;
    void*             opPostCopy;
    void*             opPostMove;
    SwagSlice         structName;
    ConcreteTypeInfo* fromGeneric;
    SwagSlice         generics;
    SwagSlice         fields;
    SwagSlice         methods;
    SwagSlice         interfaces;
    SwagSlice         attributes;
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
    SemFunctions,
    SemTypes,
    SemGlobals,
    AttributeGen,
    Max
};

enum class CompilerMsgKindMask : uint64_t
{
    PassAfterSemantic     = 1 << (uint32_t) CompilerMsgKind::PassAfterSemantic,
    PassBeforeRunByteCode = 1 << (uint32_t) CompilerMsgKind::PassBeforeRunByteCode,
    PassBeforeOutput      = 1 << (uint32_t) CompilerMsgKind::PassBeforeOutput,
    PassAllDone           = 1 << (uint32_t) CompilerMsgKind::PassAllDone,
    SemFunctions          = 1 << (uint32_t) CompilerMsgKind::SemFunctions,
    SemTypes              = 1 << (uint32_t) CompilerMsgKind::SemTypes,
    SemGlobals            = 1 << (uint32_t) CompilerMsgKind::SemGlobals,
    AttributeGen          = 1 << (uint32_t) CompilerMsgKind::AttributeGen,
    All                   = 0xFFFFFFFFFFFFFFFF,
};

struct ConcreteCompilerMessage
{
    SwagSlice         moduleName;
    CompilerMsgKind   kind;
    SwagSlice         name;
    ConcreteTypeInfo* type;
};

static const uint64_t SWAG_LAMBDA_BC_MARKER_BIT = 63;
static const uint64_t SWAG_LAMBDA_BC_MARKER     = 1ULL << SWAG_LAMBDA_BC_MARKER_BIT;

const uint32_t SWAG_COMPARE_STRICT   = 0x00000000;
const uint32_t SWAG_COMPARE_CAST_ANY = 0x00000001;
