// swag compiler mandatory file
#assert(#swagversion == 0, "'swag.swg' does not match the compiler version number")
#assert(#swagrevision == 0, "'swag.swg' does not match the compiler revision number")
#assert(#swagbuildnum == 4, "'swag.swg' does not match the compiler build number")

func @print(value: s64);
func @print(value: f64);
func @print(value: [..] u8);
func @assert(value: bool);
func @alloc(size: u32)->*void;
func @realloc(ptr: *void, size: u32)->*void;
func @free(ptr: *void);
func @memcpy(dst: *void, src: const *void, size: u32);
func @memset(dst: *void, value: u8, size: u32);
func @memcmp(dst, src: const *void, size: u32)->s32;
func @getcontext()->const *swag.Context;
func @setcontext(context: const *swag.Context);
func @args()->[..] string;
func @isbytecode()->bool;

func @sqrt(value: f32)->f32;
func @sqrt(value: f64)->f64;
func @sin(value: f32)->f32;
func @sin(value: f64)->f64;
func @cos(value: f32)->f32;
func @cos(value: f64)->f64;
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

namespace swag
{
    // #[flags] is forced by the compiler
    enum AttributeUsage : u32
    {
        Enum            = 0x00000001
        EnumValue       = 0x00000002
        Field           = 0x00000004
        GlobalVariable  = 0x00000008
        LocalVariable   = 0x00000010
        Struct          = 0x00000020
        Function        = 0x00000040
        Attribute       = 0x00000080
        Switch          = 0x00000100
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

    #[attributeUsage(AttributeUsage.Function | AttributeUsage.Switch)]
    attr complete()

    #[attributeUsage(AttributeUsage.Function)]
    attr property()

    #[attributeUsage(AttributeUsage.Function | AttributeUsage.Field)]
    attr nodoc()

    #[attributeUsage(AttributeUsage.LocalVariable)]
    attr global()

    #[attributeUsage(AttributeUsage.Function)]
    attr noreturn()

    #[attributeUsage(AttributeUsage.Function)]
    attr foreign(module: string, function: string = "");

    #[attributeUsage(AttributeUsage.Struct)]
    attr pack()

    #[attributeUsage(AttributeUsage.Field)]
    attr offset(name: string)

    #[attributeUsage(AttributeUsage.Enum)]
    attr flags()
        
    #[attributeUsage(AttributeUsage.All)]
    attr safety(value: bool)

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

    struct Context
    {
        allocator: IAllocator
    }

    // #########################################################
    // Type reflection
    // #########################################################

    enum TypeInfoKind
    {
        Invalid
        Native
        Namespace
        Enum
        FuncAttr
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
        Code
        Interface
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

    struct TypeInfo
    {
        name:	string  // Must remain first !! (see emitCompareOpEqual when comparing two typeinfos)
        kind: 	TypeInfoKind = ?
        sizeof: u32
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
        value:			*void
        attributes:	    const [..] TypeInfoAttribute
        offset:			u32
        padding:		u32
    }

    struct TypeInfoStruct
    {
        using base: 	TypeInfo
        fields:		    const [..] TypeInfoParam
        methods:	    const [..] TypeInfoParam
        interfaces:     const [..] TypeInfoParam
        attributes:	    const [..] TypeInfoAttribute
    }

    struct TypeInfoFunc
    {
        using base: 	TypeInfo
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
    // Compiler
    // #########################################################

    struct BuildCfgBackendC
    {
        maxApplicationArguments: u32    = 64
        minFunctionPerFile: u32         = 1024;
        maxFunctionPerFile: u32         = 2048;
        writeSourceCode: bool           = false
        writeByteCodeInstruction: bool  = false
    }

    struct BuildCfgBackendLLVM
    {
        minFunctionPerFile: u32 = 256;
        maxFunctionPerFile: u32 = 1024;
        outputIR: bool          = false;
    }

    struct BuildCfg
    {
        // Add guard code
        safetyGuards: bool  = true

        // Bytecode
        byteCodeMaxRecurse: u32 = 1024
        byteCodeStackSize: u32  = 16 * 1024

        // Backend common
        backendDebugInformations: bool  = false
        backendOptimizeSpeed: bool      = false
        backendOptimizeSize: bool       = false

        // Specific backend parameters
        backendC:       BuildCfgBackendC
        backendLLVM:    BuildCfgBackendLLVM
    }

    // #########################################################
    // Default compiler configuration
    // #########################################################

    // Platform setup
    #if #target == "win64"
    {
        const WINDOWS = true
        const BIG_ENDIAN = false
    }
    #else
    {
        #assert(false, "invalid target:" ~ #target)
    }
}
