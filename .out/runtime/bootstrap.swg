namespace swag
{
    // #[flags] is forced by the compiler
    enum AttributeUsage : u32
    {
        Enum            = 0x00000001
        EnumValue       = 0x00000002
        StructVariable  = 0x00000004
        GlobalVariable  = 0x00000008
        Struct          = 0x00000010
        Function        = 0x00000020
        File            = 0x00000040
        All             = 0x0FFFFFFF
    }

    // #########################################################
    // Attributes
    // #########################################################

    // Hardcoded usage
    attr attributeUsage(usage: AttributeUsage)
    attr attributeMulti()

    #[attributeUsage(AttributeUsage.Function)]
    attr constexpr()

    #[attributeUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.File)]
    attr printbc()

    #[attributeUsage(AttributeUsage.Function)]
    attr haserror()

    #[attributeUsage(AttributeUsage.Function)]
    attr compiler()

    #[attributeUsage(AttributeUsage.Function)]
    attr inline()

    #[attributeUsage(AttributeUsage.Function)]
    attr macro()

    #[attributeUsage(AttributeUsage.Function)]
    attr mixin()

    #[attributeUsage(AttributeUsage.Function)]
    attr test()

    #[attributeUsage(AttributeUsage.Function)]
    attr raiseerrors()

    // Hardcoded also for switch
    #[attributeUsage(AttributeUsage.Function)]
    attr complete()

    #[attributeUsage(AttributeUsage.Function)]
    attr property()

    // Hardcoded for type alias
    attr strict()

    // Hardcoded for local variables
    attr global()

    // Hardcoded for struct and variables
    attr align(value: u8)

    #[attributeUsage(AttributeUsage.Function)]
    attr noreturn()

    #[attributeUsage(AttributeUsage.Function)]
    attr foreign(module: string, function: string = "");

    #[attributeUsage(AttributeUsage.Struct)]
    attr pack(value: u8)

    #[attributeUsage(AttributeUsage.Struct)]
    attr nocopy()

    #[attributeUsage(AttributeUsage.StructVariable)]
    attr offset(name: string)

    #[attributeUsage(AttributeUsage.Enum)]
    attr enumflags()

    #[attributeUsage(AttributeUsage.Enum)]
    attr enumindex()

    #[attributeUsage(AttributeUsage.All)]
    attr safety(what: string, value: bool)

    #[attributeUsage(AttributeUsage.All)]
    attr selectif(value: bool)

    #[attributeUsage(AttributeUsage.Function|AttributeUsage.File)]
    attr optim(what: string, value: bool)

    #[attributeUsage(AttributeUsage.Function)]
    attr callback()

    // #########################################################
    // Constants
    // #########################################################

    const MinS8  = bitcast(s8) (-127's8 - 1)
    const MinS16 = bitcast(s16) (-32767's16 - 1)
    const MinS32 = -2147483647's32 - 1
    const MinS64 = -9223372036854775807's64 - 1
    const MinInt = MinS64

    const MaxS8  = 127's8
    const MaxS16 = 32767's16
    const MaxS32 = 2147483647's32
    const MaxS64 = 9223372036854775807's64
    const MaxInt = MaxS64

    const MaxU8   = 0xFF'u8
    const MaxU16  = 0xFFFF'u16
    const MaxU32  = 0xFFFF_FFFF'u32
    const MaxU64  = 0xFFFF_FFFF_FFFF_FFFF'u64
    const MaxUINT = MaxU64

    const MinF32 = 1.175494e-38'f32
    const MaxF32 = 3.402822e+38'f32
    const MinF64 = 2.22507e-308'f64
    const MaxF64 = 1.79769e+308'f64

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

    struct TempAllocator
    {
        block:      *u8
        capacity:   uint
        used:       uint
        lastUsed:   uint
        maxUsed:    uint
    }

    #[swag.enumflags]
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
        tempAllocator:      TempAllocator
        errorMsg:           [MaxLenErrorMsg] u8
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

    #[enumflags]
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

    #[swag.enumflags]
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
    }

    struct TypeInfo
    {
        name:	    string
        flatName:   string
        sizeof:     uint
        kind: 	    TypeInfoKind = ?
        flags:      TypeInfoFlags = ?
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
        params:		const [..]~8 TypeInfoAttributeParameter
    }

    struct TypeInfoPointer
    {
        using base: 	TypeInfo
        pointedType:	const *~8 TypeInfo
    }

    struct TypeInfoReference
    {
        using base: 	TypeInfo
        pointedType:	const *~8 TypeInfo
    }

    struct TypeInfoAlias
    {
        using base: 	TypeInfo
        rawType:		const *~8 TypeInfo
    }

    struct TypeInfoParam
    {
        name:		    string
        pointedType:	const *~8 TypeInfo
        value:			const *void
        attributes:	    const [..]~8 TypeInfoAttribute
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
        generics:		const [..]~8 TypeInfoParam
        fields:		    const [..]~8 TypeInfoParam
        methods:	    const [..]~8 TypeInfoParam
        interfaces:     const [..]~8 TypeInfoParam
        attributes:	    const [..]~8 TypeInfoAttribute
    }

    struct TypeInfoFunc
    {
        using base: 	TypeInfo
        generics:		const [..]~8 TypeInfoParam
        parameters:		const [..]~8 TypeInfoParam
        returnType:		const *~8 TypeInfo
        attributes:	    const [..]~8 TypeInfoAttribute
    }

    struct TypeInfoEnum
    {
        using base:     TypeInfo
        values:		    const [..]~8 TypeInfoParam
        rawType:	    const *~8 TypeInfo
        attributes:	    const [..]~8 TypeInfoAttribute
    }

    struct TypeInfoArray
    {
        using base: 	TypeInfo
        pointedType:	const *~8 TypeInfo
        finalType:		const *~8 TypeInfo
        count:			uint
        totalCount:		uint
    }

    struct TypeInfoSlice
    {
        using base: 	TypeInfo
        pointedType:	const *~8 TypeInfo
    }

    struct TypeInfoVariadic
    {
        using base:	TypeInfo
        rawType:	const *~8 TypeInfo
    }

    struct TypeInfoGeneric
    {
        using base: TypeInfo
        rawType:	const *~8 TypeInfo
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

    struct BuildCfg
    {
        // Module infos
        moduleVersion:  u32 = 0
        moduleRevision: u32 = 0
        moduleBuildNum: u32 = 0

        // Debug
        const SafetyNP  = 0x0000100000000000
        const SafetyBC  = 0x0000200000000000
        const SafetyOF  = 0x0000400000000000
        const SafetyMT  = 0x0000800000000000
        const SafetyAN  = 0x0001000000000000
        const SafetyAll = 0x000FF00000000000
        safetyGuards:   u64 = SafetyAll
        stackTrace: bool = true

        // Bytecode
        byteCodeOptimize: bool  = true
        byteCodeInline: bool    = true

        // Backend common
        backendDebugInformations: bool  = false
        backendOptimizeSpeed: bool      = false
        backendOptimizeSize: bool       = false

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

    #[enumflags]
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
func @getcontext()->*swag.Context;
func @setcontext(context: const *swag.Context);
func @isbytecode()->bool;
func @compiler()->swag.ICompiler;

func @alloc(size: uint)->*void;
func @realloc(ptr: *void, size: uint)->*void;
func @free(ptr: *void);
func @memset(dst: *void, value: u8, size: uint);
func @memcpy(dst: *void, src: const *void, size: uint);
func @memmove(dst: *void, src: const *void, size: uint);
func @memcmp(dst, src: const *void, size: uint)->s32;
func @cstrlen(value: const *u8)->uint;

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

// Constant intrinsics
#[swag.constexpr]
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

    func @abs(value: f32)->f32;
    func @abs(value: f64)->f64;
    func @abs(value: s8)->s8;
    func @abs(value: s16)->s16;
    func @abs(value: s32)->s32;
    func @abs(value: s64)->s64;

    func @exp(value: f32)->f32;
    func @exp(value: f64)->f64;
    func @exp2(value: f32)->f32;
    func @exp2(value: f64)->f64;
    func @pow(value1, value2: f32)->f32;
    func @pow(value1, value2: f64)->f64;
}