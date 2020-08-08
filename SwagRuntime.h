#pragma once
#include "libc/stdint.h"

extern "C" void                  swag_runtime_print_n(const void* message, swag_runtime_int32_t len);
extern "C" void                  swag_runtime_print_i64(swag_runtime_int64_t value);
extern "C" void                  swag_runtime_print_f64(double value);
extern "C" bool                  swag_runtime_comparestring(const void* str1, const void* str2, swag_runtime_uint32_t num);
extern "C" void*                 swag_runtime_loadDynamicLibrary(const void* name);
extern "C" swag_runtime_uint32_t swag_runtime_tlsAlloc();
extern "C" void                  swag_runtime_tlsSetValue(swag_runtime_uint32_t id, void* value);
extern "C" void*                 swag_runtime_tlsGetValue(swag_runtime_uint32_t id);
extern "C" void                  swag_runtime_convertArgcArgv(void* dest, swag_runtime_int32_t argc, void* argv[]);
extern "C" bool                  swag_runtime_comparetype(const void* type1, const void* type2);
extern "C" void*                 swag_runtime_interfaceof(const void* structType, const void* itfType);
extern "C" void                  swag_runtime_assert(bool expr, const void* file, swag_runtime_int32_t line, const void* msg);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BuildCfgBackendC
{
    swag_runtime_uint32_t maxApplicationArguments  = 64;
    swag_runtime_uint32_t minFunctionPerFile       = 1024;
    swag_runtime_uint32_t maxFunctionPerFile       = 2048;
    bool                  writeSourceCode          = false;
    bool                  writeByteCodeInstruction = false;
};

struct BuildCfgBackendLLVM
{
    swag_runtime_uint32_t minFunctionPerFile = 256;
    swag_runtime_uint32_t maxFunctionPerFile = 1024;
    bool                  outputIR           = false; // Write a 'file.ir' text file just next to the output file
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BuildCfg
{
    // Debug
    bool safetyGuards = true;

    // Bytecode
    swag_runtime_uint32_t byteCodeMaxRecurse = 1024;
    swag_runtime_uint32_t byteCodeStackSize  = 16 * 1024;

    // Backend common
    bool backendDebugInformations = false;
    bool backendOptimizeSpeed     = false;
    bool backendOptimizeSize      = false;

    // Specific backend parameters
    BuildCfgBackendC    backendC;
    BuildCfgBackendLLVM backendLLVM;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class TypeInfoKind
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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Should match bootstrap.swg
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct ConcreteSlice
{
    void*                 buffer;
    swag_runtime_uint64_t count;
};

struct ConcreteTypeInfo
{
    ConcreteSlice         name;
    TypeInfoKind          kind;
    swag_runtime_uint32_t sizeOf;
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
    ConcreteTypeInfo      base;
    ConcreteTypeInfo*     finalType;
    ConcreteTypeInfo*     pointedType;
    swag_runtime_uint32_t ptrCount;
};

struct ConcreteTypeInfoReference
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

struct ConcreteTypeInfoParam
{
    ConcreteSlice         name;
    ConcreteTypeInfo*     pointedType;
    void*                 value;
    ConcreteSlice         attributes;
    swag_runtime_uint32_t offsetOf;
    swag_runtime_uint32_t padding;
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
    ConcreteTypeInfo      base;
    ConcreteTypeInfo*     pointedType;
    ConcreteTypeInfo*     finalType;
    swag_runtime_uint32_t count;
    swag_runtime_uint32_t totalCount;
};

struct ConcreteTypeInfoSlice
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum class CompilerMsgKind
{
    PassBeforeSemantic,
    PassBeforeRun,
    PassBeforeOutput,
    PassAllDone,
    SemanticFunc,
};

enum class CompilerMsgKindMask : swag_runtime_uint64_t
{
    PassBeforeSemantic = 1 << (swag_runtime_uint32_t) CompilerMsgKind::PassBeforeSemantic,
    PassBeforeRun      = 1 << (swag_runtime_uint32_t) CompilerMsgKind::PassBeforeRun,
    PassBeforeOutput   = 1 << (swag_runtime_uint32_t) CompilerMsgKind::PassBeforeOutput,
    PassAllDone        = 1 << (swag_runtime_uint32_t) CompilerMsgKind::PassAllDone,
    SemanticFunc       = 1 << (swag_runtime_uint32_t) CompilerMsgKind::SemanticFunc,
    All                = 0xFFFFFFFFFFFFFFFF,
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct ConcreteCompilerMessage
{
    ConcreteSlice     moduleName;
    CompilerMsgKind   kind;
    ConcreteSlice     name;
    ConcreteTypeInfo* type;
};
