namespace Swag
{
    // #[flags] is forced by the compiler
    enum AttributeUsage : u32
    {
        Enum            = 0x00000001
        EnumValue       = 0x00000002
        StructVariable  = 0x00000004
        GlobalVariable  = 0x00000008
        Variable        = 0x00000010
        Struct          = 0x00000020
        Function        = 0x00000040
        File            = 0x00000080
        Constant        = 0x00000100
        All             = 0x0FFFFFFF
    }

    // #########################################################
    // Attributes
    // #########################################################

    // Hardcoded usage
    attr AttrUsage(usage: AttributeUsage)
    attr AttrMulti()

    #[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct)]
    attr ConstExpr()

    #[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.File)]
    attr PrintBc()

    #[AttrUsage(AttributeUsage.Function|AttributeUsage.GlobalVariable|AttributeUsage.Constant)]
    attr Compiler()

    #[AttrUsage(AttributeUsage.Function)]
    attr Inline()

    #[AttrUsage(AttributeUsage.Function)]
    attr Macro()

    #[AttrUsage(AttributeUsage.Function)]
    attr Mixin()

    #[AttrUsage(AttributeUsage.Function)]
    attr Test()

    #[AttrUsage(AttributeUsage.Function)]
    attr Implicit()

    // Hardcoded also for switch
    #[AttrUsage(AttributeUsage.Function)]
    attr Complete()

    #[AttrUsage(AttributeUsage.Function)]
    attr NoReturn()

    #[AttrUsage(AttributeUsage.Function)]
    attr Foreign(module: string, function: string = "");

    #[AttrUsage(AttributeUsage.Function)]
    attr Callback()

    #[AttrUsage(AttributeUsage.Function|AttributeUsage.Variable)]
    attr Discardable()

    #[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.Enum|AttributeUsage.EnumValue)]
    attr Deprecated(msg: string = null)

    // Hardcoded for type alias
    attr Strict()

    // Hardcoded for local variables
    attr Global()

    // Hardcoded for struct and variables
    attr Align(value: u8)

    #[AttrUsage(AttributeUsage.GlobalVariable)]
    attr Tls()

    #[AttrUsage(AttributeUsage.Struct)]
    attr Pack(value: u8)

    #[AttrUsage(AttributeUsage.Struct)]
    attr NoCopy()

    #[AttrUsage(AttributeUsage.Struct)]
    attr Opaque()

    #[AttrUsage(AttributeUsage.StructVariable)]
    attr Offset(name: string)

    #[AttrUsage(AttributeUsage.Enum)]
    attr EnumFlags()

    #[AttrUsage(AttributeUsage.Enum)]
    attr EnumIndex()

    #[AttrUsage(AttributeUsage.All)]
    attr Safety(what: string, value: bool)

    #[AttrUsage(AttributeUsage.All)]
    attr SelectIf(value: bool)

    #[AttrUsage(AttributeUsage.Function|AttributeUsage.File)]
    attr Optim(what: string, value: bool)

    // #########################################################
    // Constants
    // #########################################################

    namespace S8
    {
        const Min  = bitcast(s8) 0x80
        const Max  = bitcast(s8) 0x7F
    }

    namespace S16
    {
        const Min = bitcast(s16) 0x8000
        const Max = bitcast(s16) 0x7FFF
    }

    namespace S32
    {
        const Min = bitcast(s32) 0x80000000
        const Max = bitcast(s32) 0x7FFFFFFF
    }

    namespace S64
    {
        const Min = bitcast(s64) 0x80000000_00000000
        const Max = bitcast(s64) 0x7FFFFFFF_FFFFFFFF
    }

    namespace Int
    {
        const Min = bitcast(s64) 0x80000000_00000000
        const Max = bitcast(s64) 0x7FFFFFFF_FFFFFFFF
    }

    namespace U8
    {
        const Max = 0xFF'u8
    }

    namespace U16
    {
        const Max = 0xFFFF'u16
    }

    namespace U32
    {
        const Max = 0xFFFFFFFF'u32
    }

    namespace U64
    {
        const Max = 0xFFFFFFFF_FFFFFFFF'u64
    }

    namespace UInt
    {
        const Max = 0xFFFFFFFF_FFFFFFFF'u64
    }

    namespace F32
    {
        const Min        = 1.17549435082228750797e-38'f32
        const Max        = 3.40282346638528859812e+38'f32
        const Nan        = bitcast(f32) 0x7F800001
        const Inf        = bitcast(f32) 0x7F800000
        const NegInf     = bitcast(f32) 0xFF800000
        const MantBits   = 23
        const ExpBits    = 8
        const Bias       = -127
    }

    namespace F64
    {
        const Min        = 2.2250738585072014e-308'f64
        const Max        = 1.79769313486231570815e+308'f64
        const Nan        = bitcast(f64) 0x7ff8000000000001
        const Inf        = bitcast(f64) 0x7ff0000000000000
        const NegInf     = bitcast(f64) 0xfff0000000000000
        const MantBits   = 52
        const ExpBits    = 11
        const Bias       = -1023
    }

    // #########################################################
    // Context & allocators
    // #########################################################

    enum AllocatorMode
    {
        Alloc
        Free
        Realloc
        FreeAll
    }

    struct AllocatorRequest
    {
        callerLoc:  CompilerSourceLocation
        hint:       string
        address:	*void
        size:		uint
        oldSize:	uint
        mode =		AllocatorMode.Alloc
        alignement: u32
    }

    interface IAllocator
    {
        alloc: func(self, *AllocatorRequest)
    }

    struct ScratchAllocator
    {
        allocator:    IAllocator
        block:        *u8
        capacity:     uint
        used:         uint
        lastUsed:     uint
        maxUsed:      uint
        firstLeak:    *void
        totalLeak:    uint
        maxLeak:      uint
    }

    #[Swag.EnumFlags]
    enum ContextFlags: u64
    {
        None     = 0x00000000_00000000
        Test     = 0x00000000_00000001
        DevMode  = 0x00000000_00000002
        ByteCode = 0x00000000_00000004
    }

    const MaxLenErrorMsg = 128
    const MaxTrace = 32
    struct Context
    {
        // Must stay first, in that order, for backend generation
        allocator:      IAllocator
        flags =         ContextFlags.None

        // Other fields, can be in any order
        tempAllocator:      ScratchAllocator
        errorMsg:           [MaxLenErrorMsg] u8
        errorMsgStart:      u32
        errorMsgLen:        u32
        traceIndex:         u32
        trace:              [MaxTrace] const *CompilerSourceLocation
        exceptionLoc:       CompilerSourceLocation
        exceptionParams:    [3] const *void
        panic:              func(string, CompilerSourceLocation)
    }

    // #########################################################
    // Type reflection
    // #########################################################

    #[EnumFlags]
    enum TypeCmpFlags
    {
        Strict = 0
        CastAny = 1
    }

    enum TypeInfoKind: u8
    {
        Invalid
        Native
        Namespace
        Enum
        Func
        Param
        Lambda
        Pointer
        Reference
        Array
        Slice
        TypeListTuple
        TypeListArray
        Variadic
        TypedVariadic
        Struct
        Generic
        Alias
        NameAlias
        Code
        Interface
        TypeSet
    }

    enum TypeInfoNativeKind : u8
    {
        Void
        S8
        S16
        S32
        S64
        U8
        U16
        U32
        U64
        F32
        F64
        Bool
        Char
        String
        Any
    }

    #[Swag.EnumFlags]
    enum TypeInfoFlags: u16
    {
        None        = 0x0000
        TypeInfoPtr = 0x0001
        Integer     = 0x0002
        Float       = 0x0004
        Unsigned    = 0x0008
        HasPostCopy = 0x0010
        HasPostMove = 0x0020
        HasDrop     = 0x0040
        Strict      = 0x0080
        CanCopy     = 0x0100
    }

    struct TypeInfo
    {
        name:	    string
        flatName:   string
        sizeof:     uint
        flags:      TypeInfoFlags = ?
        kind: 	    TypeInfoKind = ?
    }

    struct TypeInfoNative
    {
        using base:     TypeInfo
        nativeKind:		TypeInfoNativeKind = TypeInfoNativeKind.Void
    }

    struct TypeInfoAttributeParameter
    {
        name:	string
        value:	any
    }

    struct TypeInfoAttribute
    {
        name:		string
        params:		const [..] TypeInfoAttributeParameter
    }

    struct TypeInfoPointer
    {
        using base: 	TypeInfo
        pointedType:	const *TypeInfo
    }

    struct TypeInfoReference
    {
        using base: 	TypeInfo
        pointedType:	const *TypeInfo
    }

    struct TypeInfoAlias
    {
        using base: 	TypeInfo
        rawType:		const *TypeInfo
    }

    struct TypeInfoParam
    {
        name:		    string
        pointedType:	const *TypeInfo
        value:			const *void
        attributes:	    const [..] TypeInfoAttribute
        offset:			u32
        padding:		u32
    }

    struct TypeInfoStruct
    {
        using base: 	TypeInfo
        opInit:         func(*void)
        opDrop:         func(*void)
        opPostCopy:     func(*void)
        opPostMove:     func(*void)
        structName:	    string
        generics:		const [..] TypeInfoParam
        fields:		    const [..] TypeInfoParam
        methods:	    const [..] TypeInfoParam
        interfaces:     const [..] TypeInfoParam
        attributes:	    const [..] TypeInfoAttribute
    }

    struct TypeInfoFunc
    {
        using base: 	TypeInfo
        generics:		const [..] TypeInfoParam
        parameters:		const [..] TypeInfoParam
        returnType:		const *TypeInfo
        attributes:	    const [..] TypeInfoAttribute
    }

    struct TypeInfoEnum
    {
        using base:     TypeInfo
        values:		    const [..] TypeInfoParam
        rawType:	    const *TypeInfo
        attributes:	    const [..] TypeInfoAttribute
    }

    struct TypeInfoArray
    {
        using base: 	TypeInfo
        pointedType:	const *TypeInfo
        finalType:		const *TypeInfo
        count:			uint
        totalCount:		uint
    }

    struct TypeInfoSlice
    {
        using base: 	TypeInfo
        pointedType:	const *TypeInfo
    }

    struct TypeInfoVariadic
    {
        using base:	TypeInfo
        rawType:	const *TypeInfo
    }

    struct TypeInfoGeneric
    {
        using base: TypeInfo
        rawType:	const *TypeInfo
    }

    // #########################################################
    // Build configuration
    // #########################################################

    struct BuildCfgBackendLLVM
    {
        minFunctionPerFile: u32 = 256;
        maxFunctionPerFile: u32 = 1024;
        outputIR: bool          = false;
    }

    struct BuildCfgBackendX64
    {
        minFunctionPerFile: u32 = 256;
        maxFunctionPerFile: u32 = 1024;
    }

    enum BuildCfgBackendKind
    {
        None
        Export
        Executable
        DynamicLib
        StaticLib
    }

    struct BuildCfg
    {
        // Module infos
        moduleVersion:      u32 = 0
        moduleRevision:     u32 = 0
        moduleBuildNum:     u32 = 0
        moduleNamespace:    string

        // Debug
        const SafetyNP  = 0x0000100000000000
        const SafetyBC  = 0x0000200000000000
        const SafetyOF  = 0x0000400000000000
        const SafetyMT  = 0x0000800000000000
        const SafetyAN  = 0x0001000000000000
        const SafetyAll = 0x000FF00000000000
        safetyGuards:   u64 = SafetyAll
        stackTrace = true

        // Bytecode
        byteCodeOptimize        = true
        byteCodeDebugInline     = true
        byteCodeEmitAssume      = true
        byteCodeInline          = true

        // Backend common
        backendKind               = BuildCfgBackendKind.Executable
        backendDebugInformations  = false
        backendOptimizeSpeed      = false
        backendOptimizeSize       = false

        // Specific backend parameters
        backendLLVM:    BuildCfgBackendLLVM
        backendX64:     BuildCfgBackendX64
    }

    // #########################################################
    // Compiler
    // #########################################################

    enum CompilerMsgKind : u32
    {
        PassAfterSemantic
        PassBeforeRunByteCode
        PassBeforeOutput
        PassAllDone
        SemanticFunc
	}

    #[EnumFlags]
    enum CompilerMsgMask : u64
    {
        PassAfterSemantic =     1 << cast(u32) CompilerMsgKind.PassAfterSemantic
        PassBeforeRun =         1 << cast(u32) CompilerMsgKind.PassBeforeRunByteCode
        PassBeforeOutput =      1 << cast(u32) CompilerMsgKind.PassBeforeOutput
        PassAllDone =           1 << cast(u32) CompilerMsgKind.PassAllDone
        SemanticFunc =          1 << cast(u32) CompilerMsgKind.SemanticFunc
        All =                   0xFFFFFFFF_FFFFFFFF
	}

    struct CompilerMessage
    {
        moduleName:     string
        kind:           CompilerMsgKind = ?
	    name:           string
        type:           const* TypeInfo
    }

    interface ICompiler
    {
        getMessage:     func(self)->const* CompilerMessage
        getBuildCfg:    func(self)->*BuildCfg
        compileString:  func(self, string)
    }

    struct CompilerSourceLocation
    {
        fileName:               string
        lineStart, colStart:    u32
        lineEnd, colEnd:        u32
    }
}

//
// Intrinsic
//

func @assert(value: bool);
func @bcdbg();
func @getcontext()->*Swag.Context;
func @setcontext(context: const *Swag.Context);
func @isbytecode()->bool;
func @compiler()->Swag.ICompiler;

func @alloc(size: uint)->*void;
func @realloc(ptr: *void, size: uint)->*void;
func @free(ptr: *void);
func @memset(dst: *void, value: u8, size: uint);
func @memcpy(dst: *void, src: const *void, size: uint);
func @memmove(dst: *void, src: const *void, size: uint);
func @memcmp(dst, src: const *void, size: uint)->s32;
func @cstrlen(value: const *u8)->uint;

// Atomic operations
#[Swag.Discardable]
{
    func @atomadd(addr: *s8, value: s8)->s8;
    func @atomadd(addr: *s16, value: s16)->s16;
    func @atomadd(addr: *s32, value: s32)->s32;
    func @atomadd(addr: *s64, value: s64)->s64;
    func @atomadd(addr: *u8, value: u8)->u8;
    func @atomadd(addr: *u16, value: u16)->u16;
    func @atomadd(addr: *u32, value: u32)->u32;
    func @atomadd(addr: *u64, value: u64)->u64;

    func @atomand(addr: *s8, value: s8)->s8;
    func @atomand(addr: *s16, value: s16)->s16;
    func @atomand(addr: *s32, value: s32)->s32;
    func @atomand(addr: *s64, value: s64)->s64;
    func @atomand(addr: *u8, value: u8)->u8;
    func @atomand(addr: *u16, value: u16)->u16;
    func @atomand(addr: *u32, value: u32)->u32;
    func @atomand(addr: *u64, value: u64)->u64;

    func @atomor(addr: *s8, value: s8)->s8;
    func @atomor(addr: *s16, value: s16)->s16;
    func @atomor(addr: *s32, value: s32)->s32;
    func @atomor(addr: *s64, value: s64)->s64;
    func @atomor(addr: *u8, value: u8)->u8;
    func @atomor(addr: *u16, value: u16)->u16;
    func @atomor(addr: *u32, value: u32)->u32;
    func @atomor(addr: *u64, value: u64)->u64;

    func @atomxor(addr: *s8, value: s8)->s8;
    func @atomxor(addr: *s16, value: s16)->s16;
    func @atomxor(addr: *s32, value: s32)->s32;
    func @atomxor(addr: *s64, value: s64)->s64;
    func @atomxor(addr: *u8, value: u8)->u8;
    func @atomxor(addr: *u16, value: u16)->u16;
    func @atomxor(addr: *u32, value: u32)->u32;
    func @atomxor(addr: *u64, value: u64)->u64;

    func @atomxchg(addr: *s8,  exchangeWith: s8)->s8;
    func @atomxchg(addr: *s16, exchangeWith: s16)->s16;
    func @atomxchg(addr: *s32, exchangeWith: s32)->s32;
    func @atomxchg(addr: *s64, exchangeWith: s64)->s64;
    func @atomxchg(addr: *u8,  exchangeWith: u8)->u8;
    func @atomxchg(addr: *u16, exchangeWith: u16)->u16;
    func @atomxchg(addr: *u32, exchangeWith: u32)->u32;
    func @atomxchg(addr: *u64, exchangeWith: u64)->u64;

    func @atomcmpxchg(addr: *s8,  compareTo, exchangeWith: s8)->s8;
    func @atomcmpxchg(addr: *s16, compareTo, exchangeWith: s16)->s16;
    func @atomcmpxchg(addr: *s32, compareTo, exchangeWith: s32)->s32;
    func @atomcmpxchg(addr: *s64, compareTo, exchangeWith: s64)->s64;
    func @atomcmpxchg(addr: *u8,  compareTo, exchangeWith: u8)->u8;
    func @atomcmpxchg(addr: *u16, compareTo, exchangeWith: u16)->u16;
    func @atomcmpxchg(addr: *u32, compareTo, exchangeWith: u32)->u32;
    func @atomcmpxchg(addr: *u64, compareTo, exchangeWith: u64)->u64;
}

// Constant intrinsics
#[Swag.ConstExpr]
{
    func @sqrt(value: f32)->f32;
    func @sqrt(value: f64)->f64;

    func @sin(value: f32)->f32;
    func @sin(value: f64)->f64;
    func @cos(value: f32)->f32;
    func @cos(value: f64)->f64;
    func @tan(value: f32)->f32;
    func @tan(value: f64)->f64;

    func @sinh(value: f32)->f32;
    func @sinh(value: f64)->f64;
    func @cosh(value: f32)->f32;
    func @cosh(value: f64)->f64;
    func @tanh(value: f32)->f32;
    func @tanh(value: f64)->f64;

    func @asin(value: f32)->f32;
    func @asin(value: f64)->f64;
    func @acos(value: f32)->f32;
    func @acos(value: f64)->f64;
    func @atan(value: f32)->f32;
    func @atan(value: f64)->f64;

    func @log(value: f32)->f32;
    func @log(value: f64)->f64;
    func @log2(value: f32)->f32;
    func @log2(value: f64)->f64;
    func @log10(value: f32)->f32;
    func @log10(value: f64)->f64;

    func @floor(value: f32)->f32;
    func @floor(value: f64)->f64;
    func @ceil(value: f32)->f32;
    func @ceil(value: f64)->f64;
    func @trunc(value: f32)->f32;
    func @trunc(value: f64)->f64;
    func @round(value: f32)->f32;
    func @round(value: f64)->f64;

    func @abs(value: s8)->s8;
    func @abs(value: s16)->s16;
    func @abs(value: s32)->s32;
    func @abs(value: s64)->s64;
    func @abs(value: f32)->f32;
    func @abs(value: f64)->f64;

    func @exp(value: f32)->f32;
    func @exp(value: f64)->f64;
    func @exp2(value: f32)->f32;
    func @exp2(value: f64)->f64;
    func @pow(value1, value2: f32)->f32;
    func @pow(value1, value2: f64)->f64;
    func @atan2(value1, value2: f32)->f32;
    func @atan2(value1, value2: f64)->f64;

    func @min(value1, value2: s8)->s8;
    func @min(value1, value2: s16)->s16;
    func @min(value1, value2: s32)->s32;
    func @min(value1, value2: s64)->s64;
    func @min(value1, value2: u8)->u8;
    func @min(value1, value2: u16)->u16;
    func @min(value1, value2: u32)->u32;
    func @min(value1, value2: u64)->u64;
    func @min(value1, value2: f32)->f32;
    func @min(value1, value2: f64)->f64;

    func @max(value1, value2: s8)->s8;
    func @max(value1, value2: s16)->s16;
    func @max(value1, value2: s32)->s32;
    func @max(value1, value2: s64)->s64;
    func @max(value1, value2: u8)->u8;
    func @max(value1, value2: u16)->u16;
    func @max(value1, value2: u32)->u32;
    func @max(value1, value2: u64)->u64;
    func @max(value1, value2: f32)->f32;
    func @max(value1, value2: f64)->f64;

    func @bitcountnz(value: u8)->u8;
    func @bitcountnz(value: u16)->u16;
    func @bitcountnz(value: u32)->u32;
    func @bitcountnz(value: u64)->u64;
    func @bitcounttz(value: u8)->u8;
    func @bitcounttz(value: u16)->u16;
    func @bitcounttz(value: u32)->u32;
    func @bitcounttz(value: u64)->u64;
    func @bitcountlz(value: u8)->u8;
    func @bitcountlz(value: u16)->u16;
    func @bitcountlz(value: u32)->u32;
    func @bitcountlz(value: u64)->u64;

    func @byteswap(value: u16)->u16;
    func @byteswap(value: u32)->u32;
    func @byteswap(value: u64)->u64;
}