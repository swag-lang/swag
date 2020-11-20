func @assert(value: bool);
func @alloc(size: u32)->*void;
func @realloc(ptr: *void, size: u32)->*void;
func @free(ptr: *void);
func @memset(dst: *void, value: u8, size: u32);
func @getcontext()->const *swag.Context;
func @setcontext(context: const *swag.Context);
func @isbytecode()->bool;
func @thrunptr()->const *void;
func @compiler()->swag.ICompiler;

namespace swag
{
    // #[flags] is forced by the compiler
    enum AttributeUsage : u32
    {
        Enum            = 0x00000001
        EnumValue       = 0x00000002
        Field           = 0x00000004
        GlobalVariable  = 0x00000008
        Struct          = 0x00000010
        Function        = 0x00000020
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

    #[attributeUsage(AttributeUsage.Function)]
    attr printbc()

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

    // Hardcoded also for switch
    #[attributeUsage(AttributeUsage.Function)]
    attr complete()

    #[attributeUsage(AttributeUsage.Function)]
    attr property()

    // Hardcoded for local variables
    attr global()

    #[attributeUsage(AttributeUsage.Function)]
    attr noreturn()

    #[attributeUsage(AttributeUsage.Function)]
    attr foreign(module: string, function: string = "");

    #[attributeUsage(AttributeUsage.Struct)]
    attr pack(value: u8)

    #[attributeUsage(AttributeUsage.Field)]
    attr offset(name: string)

    #[attributeUsage(AttributeUsage.Enum)]
    attr enumflags()

    #[attributeUsage(AttributeUsage.Enum)]
    attr enumindex()

    #[attributeUsage(AttributeUsage.All)]
    attr safety(value: bool)

    #[attributeUsage(AttributeUsage.Function)]
    attr optim(value: bool)

    #[attributeUsage(AttributeUsage.Function)]
    attr callback()

    // #########################################################
    // Constants
    // #########################################################

    const MinS8  = -127's8 - 1
    const MinS16 = -32767's16 - 1
    const MinS32 = -2147483647's32 - 1
    const MinS64 = -9223372036854775807's64 - 1

    const MaxS8  = 127's8
    const MaxS16 = 32767's16
    const MaxS32 = 2147483647's32
    const MaxS64 = 9223372036854775807's64

    const MaxU8  = 0xFF'u8
    const MaxU16 = 0xFFFF'u16
    const MaxU32 = 0xFFFF_FFFF'u32
    const MaxU64 = 0xFFFF_FFFF_FFFF_FFFF'u64

    const MinF32 = 1.175494e-38'f32
    const MaxF32 = 3.402822e+38'f32
    const MinF64 = 2.22507e-308'f64
    const MaxF64 = 1.79769e+308'f64

    const LambdaBcMarker      = 0x8000000000000000'u64
    const LambdaForeignMarker = 0x0400000000000000'u64

    // #########################################################
    // Context & allocators
    // #########################################################

    enum AllocatorMode
    {
        Alloc
        Free
        Realloc
    }

    struct AllocatorRequest
    {
        mode:		AllocatorMode = AllocatorMode.Alloc
        size:		u32
        address:	*void
    }

    interface IAllocator
    {
        alloc: func(self, *AllocatorRequest)
    }

    struct SystemAllocator
    {
    }

    impl IAllocator for SystemAllocator
    {
        func alloc(self, request: *AllocatorRequest)
        {
            switch request.mode
            {
                case AllocatorMode.Alloc:
                    request.address = @alloc(request.size)
                case AllocatorMode.Free:
                    @free(request.address)
                case AllocatorMode.Realloc:
                    request.address = @realloc(request.address, request.size)
            }
        }
    }

    #[swag.enumflags]
    enum ContextFlags: u64
    {
        None     = 0x00000000_00000000
        Test     = 0x00000000_00000001
        DevMode  = 0x00000000_00000002
        ByteCode = 0x00000000_00000004
    }

    struct Context
    {
        allocator:  IAllocator
        flags =     ContextFlags.None
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

    enum TypeInfoKind: u16
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

    enum TypeInfoNativeKind
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
    }

    struct TypeInfo
    {
        name:	string  // Must remain first !! (see emitCompareOpEqual when comparing two typeinfos)
        sizeof: u32
        kind: 	TypeInfoKind = ?
        flags:  TypeInfoFlags = ?
    }

    struct TypeInfoNative
    {
        using base:     TypeInfo
        nativeKind:		TypeInfoNativeKind = TypeInfoNativeKind.Void
    }

    struct TypeInfoPointer
    {
        using base: 	TypeInfo
        finalType:		const *TypeInfo
        pointedType:	const *TypeInfo
        ptrCount:		u32
    }

    struct TypeInfoAlias
    {
        using base: 	TypeInfo
        rawType:		const *TypeInfo
    }

    struct TypeInfoReference
    {
        using base: 	TypeInfo
        pointedType:	const *TypeInfo
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
        rawType:		const *TypeInfo
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

    struct TypeInfoVariadic
    {
        using base:	TypeInfo
    }

    struct TypeInfoArray
    {
        using base: 	TypeInfo
        pointedType:	const *TypeInfo
        finalType:		const *TypeInfo
        count:			u32
        totalCount:		u32
    }

    struct TypeInfoSlice
    {
        using base: 	TypeInfo
        pointedType:	const *TypeInfo
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

    struct BuildCfg
    {
        // Add guard code
        safetyGuards: bool  = true

        // Bytecode
        byteCodeMaxRecurse: u32 = 1024
        byteCodeStackSize: u32  = 16 * 1024
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
        PassBeforePublish
        PassBeforeSemantic
        PassAfterSemantic
        PassBeforeRun
        PassBeforeOutput
        PassAllDone
        SemanticFunc
	}

    #[enumflags]
    enum CompilerMsgMask : u64
    {
        PassBeforePublish =     1 << cast(u32) CompilerMsgKind.PassBeforePublish
        PassBeforeSemantic =    1 << cast(u32) CompilerMsgKind.PassBeforeSemantic
        PassAfterSemantic =     1 << cast(u32) CompilerMsgKind.PassAfterSemantic
        PassBeforeRun =         1 << cast(u32) CompilerMsgKind.PassBeforeRun
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