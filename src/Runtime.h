#pragma once

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

constexpr uint64_t SAFETY_BOUND_CHECK = 0x0001;
constexpr uint64_t SAFETY_OVERFLOW    = 0x0002;
constexpr uint64_t SAFETY_MATH        = 0x0004;
constexpr uint64_t SAFETY_ANY         = 0x0008;
constexpr uint64_t SAFETY_SWITCH      = 0x0010;
constexpr uint64_t SAFETY_BOOL        = 0x0020;
constexpr uint64_t SAFETY_NAN         = 0x0040;
constexpr uint64_t SAFETY_SANITY      = 0x0080;
constexpr uint64_t SAFETY_UNREACHABLE = 0x0100;
constexpr uint64_t SAFETY_NULL        = 0x0200;
constexpr uint64_t SAFETY_ALL         = 0xFFFF;

constexpr int SWAG_EXCEPTION_TO_PREV_HANDLER     = 665;
constexpr int SWAG_EXCEPTION_TO_COMPILER_HANDLER = 666; // must be the same value in __raiseException666 in runtime_windows.h

enum WarnLevel : uint8_t
{
	Enable,
	Disable,
	Error,
};

enum AttributeUsage
{
	// Usage
	Enum              = 0x00000001,
	EnumValue         = 0x00000002,
	StructVariable    = 0x00000004,
	GlobalVariable    = 0x00000008,
	Variable          = 0x00000010,
	Struct            = 0x00000020,
	Function          = 0x00000040,
	FunctionParameter = 0x00000080,
	File              = 0x00000100,
	Constant          = 0x00000200,
	MaskType          = 0x00FFFFFF,
	// Flags
	Multi = 0x01000000,
	Gen   = 0x02000000,
	All   = 0x04000000,
};

enum class ContextFlags : uint64_t
{
	Test                   = 0x00000000'00000001,
	ByteCode               = 0x00000000'00000002,
	PostCopyResetAllocator = 0x00000000'00000004,
};

using SwagInterface = struct SwagInterface
{
	void* data   = nullptr;
	void* itable = nullptr;
};

using SwagTempAllocator = struct SwagScratchAllocator
{
	SwagInterface allocator;
	void*         data      = nullptr;
	uint64_t      capacity  = 0;
	uint64_t      used      = 0;
	uint64_t      maxUsed   = 0;
	void*         firstLeak = nullptr;
	uint64_t      totalLeak = 0;
	uint64_t      maxLeak   = 0;
};

using SwagSlice = struct SwagSlice
{
	void*    buffer = nullptr;
	uint64_t count  = 0;
};

struct SwagAny
{
	void*                    value;
	struct ExportedTypeInfo* type;
};

struct SwagSourceCodeLocation
{
	SwagSlice fileName;
	uint32_t  lineStart, colStart;
	uint32_t  lineEnd,   colEnd;
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
	void*    ptr;
	void*    opDrop;
	uint32_t sizeOf;
	uint32_t count;
};

enum class SwagExceptionKind
{
	Panic,
	Error,
	Warning,
};

constexpr auto SWAG_MAX_ERRORS = 32;
constexpr auto SWAG_MAX_TRACES = 32;

using SwagError = struct SwagErrorValue
{
	SwagAny  value;
	uint32_t pushUsedAlloc;
	uint32_t pushTraceIndex;
};

using SwagContext = struct SwagContext
{
	SwagInterface           allocator;
	uint64_t                flags;
	SwagScratchAllocator    tempAllocator;
	SwagScratchAllocator    errorAllocator;
	void*                   debugAllocator;
	uint64_t                runtimeFlags;
	uint64_t                user0;
	uint64_t                user1;
	uint64_t                user2;
	uint64_t                user3;
	SwagSourceCodeLocation* traces[SWAG_MAX_TRACES];
	SwagErrorValue          errors[SWAG_MAX_ERRORS];
	SwagSourceCodeLocation  exceptionLoc;
	void*                   exceptionParams[4];
	void*                   panic;
	SwagAny                 curError;
	uint32_t                errorIndex;
	uint32_t                traceIndex;
	uint32_t                hasError;
};

using FuncCB = void* (*)(void*, void*, void*, void*, void*, void*);

using SwagBytecodeRun = void(*)(void*, ...);
using SwagThreadRun = void(*)(void*);
using SwagMakeCallback = FuncCB(*)(void*);

enum class SwagBackendGenType : uint32_t
{
	ByteCode,
	SCBE,
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

using SwagProcessInfos = struct SwagProcessInfos
{
	SwagSlice          modules        = {nullptr};
	SwagSlice          args           = {nullptr};
	uint64_t           contextTlsId   = 0;
	SwagContext*       defaultContext = nullptr;
	SwagBytecodeRun    byteCodeRun    = nullptr;
	SwagMakeCallback   makeCallback   = nullptr;
	SwagBackendGenType backendKind    = SwagBackendGenType::SCBE;
};

struct BuildCfgLLVM
{
	bool outputIR = false; // Write a '.ir' text file just next to the temporary file

	bool fpMathFma          = false;
	bool fpMathNoNaN        = false;
	bool fpMathNoInf        = false;
	bool fpMathNoSignedZero = false;
	bool fpMathUnsafe       = false;
	bool fpMathApproxFunc   = false;
};

struct BuildCfgSCBE
{
};

enum class BuildCfgBackendKind
{
	None,
	Export,
	Executable,
	Library,
};

enum class BuildCfgOutputKind
{
	Executable,
	DynamicLib,
	ImportLib,
	StaticLib,
};

enum class BuildCfgBackendSubKind
{
	Default,
	Console,
};

enum class BuildCfgBackendOptim
{
	O0,
	O1,
	O2,
	O3,
	Os,
	Oz,
};

enum class BuildCfgDocKind
{
	None,
	Api,
	Examples,
	Pages,
};

struct BuildCfgGenDoc
{
	BuildCfgDocKind kind = BuildCfgDocKind::None;
	SwagSlice       outputName;
	SwagSlice       outputExtension;
	SwagSlice       titleToc;
	SwagSlice       titleContent;
	SwagSlice       css;
	SwagSlice       icon;
	SwagSlice       startHead;
	SwagSlice       endHead;
	SwagSlice       startBody;
	SwagSlice       endBody;
	SwagSlice       morePages;
	SwagSlice       quoteIconNote;
	SwagSlice       quoteIconTip;
	SwagSlice       quoteIconWarning;
	SwagSlice       quoteIconAttention;
	SwagSlice       quoteIconExample;
	SwagSlice       quoteTitleNote;
	SwagSlice       quoteTitleTip;
	SwagSlice       quoteTitleWarning;
	SwagSlice       quoteTitleAttention;
	SwagSlice       quoteTitleExample;
	uint32_t        syntaxDefaultColor = 0x00222222;
	float           syntaxColorLum     = 0.5f;
	bool            hasFontAwesome     = true;
	bool            hasStyleSection    = true;
	bool            hasSwagWatermark   = true;
};

struct BuildCfg
{
	// Module information
	uint32_t  moduleVersion  = 0;
	uint32_t  moduleRevision = 0;
	uint32_t  moduleBuildNum = 0;
	SwagSlice moduleNamespace;
	bool      embeddedImports = false;

	// Debug
	uint32_t tempAllocatorCapacity      = 4 * 1024 * 1024;
	uint32_t errorAllocatorCapacity     = 16 * 1024;
	uint16_t safetyGuards               = SAFETY_ALL;
	bool     debugAllocator             = true;
	bool     debugAllocatorCaptureStack = true;
	bool     debugAllocatorLeaks        = true;
	bool     errorStackTrace            = true;

	// Warnings
	SwagSlice warnAsErrors;
	SwagSlice warnAsWarnings;
	SwagSlice warnAsDisabled;
	bool      warnDefaultDisabled = false;
	bool      warnDefaultErrors   = false;

	// Bytecode
	uint32_t byteCodeOptimizeLevel = 1;
	bool     byteCodeEmitAssume    = true;
	bool     byteCodeInline        = true;
	bool     byteCodeAutoInline    = true;

	// Backend common
	BuildCfgBackendKind    backendKind        = BuildCfgBackendKind::Executable;
	BuildCfgBackendSubKind backendSubKind     = BuildCfgBackendSubKind::Console;
	bool                   backendDebugInfos  = false;
	bool                   backendDebugInline = false;
	BuildCfgBackendOptim   backendOptimize    = BuildCfgBackendOptim::O0;
	uint32_t               backendNumCU       = 0;

	// Linker
	SwagSlice linkerArgs;

	// Specific backend parameters
	BuildCfgLLVM backendLLVM;
	BuildCfgSCBE backendSCBE;

	// Paths
	SwagSlice repoPath;

	// Documentation
	BuildCfgGenDoc genDoc;
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
	Attribute,
	// Only for export
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
	Count,
};

enum class ExportedTypeInfoFlags : uint32_t
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
	PointerMoveRef    = 0x00002000,
	PointerArithmetic = 0x00004000,
	Character         = 0x00008000,
	Const             = 0x00010000,
};

struct ExportedTypeInfo
{
	SwagSlice    fullName;
	SwagSlice    name;
	uint32_t     sizeOf;
	uint32_t     crc32;
	uint32_t     flags;
	TypeInfoKind kind;
	uint8_t      padding[3];
};

struct ExportedAttributeParameter
{
	SwagSlice name;
	SwagAny   value;
};

struct ExportedAttribute
{
	ExportedTypeInfo* type;
	SwagSlice         params;
};

struct ExportedTypeValue
{
	SwagSlice         name;
	ExportedTypeInfo* pointedType;
	void*             value;
	SwagSlice         attributes;
	uint32_t          offsetOf;
	uint32_t          crc32;
};

struct ExportedTypeInfoNative
{
	ExportedTypeInfo base;
	NativeTypeKind   nativeKind;
};

struct ExportedTypeInfoPointer
{
	ExportedTypeInfo  base;
	ExportedTypeInfo* pointedType;
};

struct ExportedTypeInfoAlias
{
	ExportedTypeInfo  base;
	ExportedTypeInfo* rawType;
};

struct ExportedTypeInfoNamespace
{
	ExportedTypeInfo base;
};

struct ExportedTypeInfoStruct
{
	ExportedTypeInfo  base;
	void*             opInit;
	void*             opDrop;
	void*             opPostCopy;
	void*             opPostMove;
	SwagSlice         structName;
	ExportedTypeInfo* fromGeneric;
	SwagSlice         generics;
	SwagSlice         fields;
	SwagSlice         methods;
	SwagSlice         interfaces;
	SwagSlice         attributes;
};

struct ExportedTypeInfoFunc
{
	ExportedTypeInfo  base;
	SwagSlice         generics;
	SwagSlice         parameters;
	ExportedTypeInfo* returnType;
	SwagSlice         attributes;
};

struct ExportedTypeInfoEnum
{
	ExportedTypeInfo  base;
	SwagSlice         values;
	ExportedTypeInfo* rawType;
	SwagSlice         attributes;
};

struct ExportedTypeInfoArray
{
	ExportedTypeInfo  base;
	ExportedTypeInfo* pointedType;
	ExportedTypeInfo* finalType;
	uint64_t          count;
	uint64_t          totalCount;
};

struct ExportedTypeInfoSlice
{
	ExportedTypeInfo  base;
	ExportedTypeInfo* pointedType;
};

struct ExportedTypeInfoVariadic
{
	ExportedTypeInfo  base;
	ExportedTypeInfo* rawType;
};

struct ExportedTypeInfoGeneric
{
	ExportedTypeInfo  base;
	ExportedTypeInfo* rawType;
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
	PassAfterSemantic     = 1 << static_cast<uint32_t>(CompilerMsgKind::PassAfterSemantic),
	PassBeforeRunByteCode = 1 << static_cast<uint32_t>(CompilerMsgKind::PassBeforeRunByteCode),
	PassBeforeOutput      = 1 << static_cast<uint32_t>(CompilerMsgKind::PassBeforeOutput),
	PassAllDone           = 1 << static_cast<uint32_t>(CompilerMsgKind::PassAllDone),
	SemFunctions          = 1 << static_cast<uint32_t>(CompilerMsgKind::SemFunctions),
	SemTypes              = 1 << static_cast<uint32_t>(CompilerMsgKind::SemTypes),
	SemGlobals            = 1 << static_cast<uint32_t>(CompilerMsgKind::SemGlobals),
	AttributeGen          = 1 << static_cast<uint32_t>(CompilerMsgKind::AttributeGen),
	All                   = 0xFFFFFFFFFFFFFFFF,
};

struct ExportedCompilerMessage
{
	SwagSlice         moduleName;
	SwagSlice         name;
	ExportedTypeInfo* type = nullptr;
	CompilerMsgKind   kind = CompilerMsgKind::Max;
};

static constexpr uint64_t SWAG_LAMBDA_BC_MARKER_BIT = 63;
static constexpr uint64_t SWAG_LAMBDA_BC_MARKER     = 1ULL << SWAG_LAMBDA_BC_MARKER_BIT;

constexpr uint32_t SWAG_COMPARE_STRICT   = 0x00000000;
constexpr uint32_t SWAG_COMPARE_CAST_ANY = 0x00000001;

inline void setSlice(SwagSlice& slice, const char* value)
{
	slice.buffer = (void*) value;
	slice.count  = strlen(value);
}
