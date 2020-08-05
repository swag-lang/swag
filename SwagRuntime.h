#pragma once

extern "C" void*    swag_runtime_malloc(uint64_t size);
extern "C" void*    swag_runtime_realloc(void* addr, uint64_t size);
extern "C" void     swag_runtime_free(void* addr);
extern "C" int32_t  swag_runtime_memcmp(const void* mem1, const void* mem2, uint64_t size);
extern "C" void     swag_runtime_exit(int32_t exitCode);
extern "C" void     swag_runtime_print_n(const void* message, int len);
extern "C" int32_t  swag_runtime_strlen(const void* message);
extern "C" void     swag_runtime_print(const void* message);
extern "C" void     swag_runtime_print_i64(int64_t value);
extern "C" void     swag_runtime_print_f64(double value);
extern "C" bool     swag_runtime_strcmp(const void* str1, const void* str2, uint32_t num);
extern "C" void*    swag_runtime_loadDynamicLibrary(const void* name);
extern "C" uint32_t swag_runtime_tlsAlloc();
extern "C" void     swag_runtime_tlsSetValue(uint32_t id, void* value);
extern "C" void*    swag_runtime_tlsGetValue(uint32_t id);
extern "C" void     swag_runtime_convertArgcArgv(void* dest, int argc, void* argv[]);
extern "C" bool     swag_runtime_comparetype(const void* type1, const void* type2);
extern "C" void*    swag_runtime_interfaceof(const void* structType, const void* itfType);
extern "C" void     swag_runtime_assert(bool expr, const void* file, int line, const void* msg);

#define SWAG_MAX_COMMAND_ARGUMENTS 512

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BuildCfgBackendC
{
    uint32_t maxApplicationArguments  = 64;
    uint32_t minFunctionPerFile       = 1024;
    uint32_t maxFunctionPerFile       = 2048;
    bool     writeSourceCode          = false;
    bool     writeByteCodeInstruction = false;
};

struct BuildCfgBackendLLVM
{
    uint32_t minFunctionPerFile = 256;
    uint32_t maxFunctionPerFile = 1024;
    bool     outputIR           = false; // Write a 'file.ir' text file just next to the output file
};

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN SWAG.BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct BuildCfg
{
    // Debug
    bool safetyGuards = true;

    // Bytecode
    uint32_t byteCodeMaxRecurse = 1024;
    uint32_t byteCodeStackSize  = 16 * 1024;

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
    void*    buffer;
    uint64_t count;
};

struct ConcreteTypeInfo
{
    ConcreteSlice name;
    TypeInfoKind  kind;
    uint32_t      sizeOf;
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
