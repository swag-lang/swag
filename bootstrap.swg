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
func @target()->*swag.CompilerTarget;
func @isbytecode()->bool;

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
        Attribute       = 0x00000040
        All             = 0x0FFFFFFF
    }

	// #########################################################
	// Attributes
	// #########################################################

    // Hardcoded usage
    attr attributeUsage(usage: AttributeUsage);
    attr attributeMulti();

    #[attributeUsage(AttributeUsage.Function)]
	attr constexpr();

    #[attributeUsage(AttributeUsage.Function)]
	attr printbc();

    #[attributeUsage(AttributeUsage.Function)]
	attr compiler();

    #[attributeUsage(AttributeUsage.Function)]
	attr inline();

    #[attributeUsage(AttributeUsage.Function)]
    attr macro();

    #[attributeUsage(AttributeUsage.Function)]
    attr mixin();

    #[attributeUsage(AttributeUsage.Function)]
	attr test();

    #[attributeUsage(AttributeUsage.Function)]
    attr complete();

    #[attributeUsage(AttributeUsage.Function)]
	attr property();

    #[attributeUsage(AttributeUsage.Function)]
	attr nodoc();

    #[attributeUsage(AttributeUsage.Function)]
	attr foreign(module: string, function: string = "");

    #[attributeUsage(AttributeUsage.Struct)]
	attr pack();

    #[attributeUsage(AttributeUsage.Field)]
    attr offset(name: string);

    #[attributeUsage(AttributeUsage.Enum)]
	attr flags();

    #[attributeUsage(AttributeUsage.GlobalVariable)]
	attr nobss();
	
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
		Array
		Slice
		TypeList
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
		name:	string
		kind: 	TypeInfoKind = ?
		sizeof: u32
	}

	struct TypeInfoNative
	{
		base: 			TypeInfo
		nativeKind:		TypeInfoNativeKind = TypeInfoNativeKind.Void
	}

	struct TypeInfoPointer
	{
		base: 			TypeInfo
		finalType:		const *TypeInfo
		pointedType:	const *TypeInfo
		ptrCount:		u32
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
		base: 		TypeInfo
		fields:		const [..] TypeInfoParam
		methods:	const [..] TypeInfoParam
        interfaces: const [..] TypeInfoParam
		attributes:	const [..] TypeInfoAttribute
	}
    
	struct TypeInfoFunc
	{
		base: 			TypeInfo
		parameters:		const [..] TypeInfoParam
		returnType:		const *TypeInfo
		attributes:	    const [..] TypeInfoAttribute
	}

	struct TypeInfoEnum
	{
		base: 		TypeInfo
		values:		const [..] TypeInfoParam
		rawType:	const *TypeInfo
		attributes:	const [..] TypeInfoAttribute
	}

	struct TypeInfoVariadic
	{
		base:	TypeInfo
	}

	struct TypeInfoArray
	{
		base: 			TypeInfo
		pointedType:	const *TypeInfo
		finalType:		const *TypeInfo
		count:			u32
		totalCount:		u32
	}

	struct TypeInfoSlice
	{
		base: 			TypeInfo
		pointedType:	const *TypeInfo
	}

	struct TypeInfoGeneric
	{
		base: 		TypeInfo
		rawType:	const *TypeInfo
	}
    
	// #########################################################
	// Compiler
	// #########################################################
    
    struct CompilerParamsBackendC
    {
        writeSourceCode: bool           = false
        writeByteCodeInstruction: bool  = false
        maxApplicationArguments: u32    = 64
    }

    struct CompilerTarget
    {
        // Debug
        debugBoundCheck: bool    = true
        debugAnyCastCheck: bool  = true
        debugDivZeroCheck: bool  = true
        
        // Bytecode
        byteCodeMaxRecurse: u32     = 1024
        byteCodeStackSize: u32      = 16 * 1024

        // Backend common
        backendDebugInformations: bool  = false
        backendOptimizeLevel: u32       = 0
        
        // Backend specific
        backendC: CompilerParamsBackendC
    }
        
	// #########################################################
	// Default compiler configuration
	// #########################################################
    
    // Platform setup
    #if #arch == "win64"
    {
        const WINDOWS = true
        const BIG_ENDIAN = false
    }
    #else
    {
        #assert(false, "invalid platform:" ~ #arch)
    }

    // Configuration setup
    #if #config == "debug"
    {
        const DEBUG = true
        const ASSERT = true
    }
    #elif #config == "release"
    {
        const DEBUG = false
        const ASSERT = false
    }
    #else
    {
        const DEBUG = false
        const ASSERT = false
    }    
}

