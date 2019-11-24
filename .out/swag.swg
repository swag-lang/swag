// swag compiler mandatory file
#assert(#swagversion == 0, "'swag.swg' does not match the compiler version number")
#assert(#swagrevision == 0, "'swag.swg' does not match the compiler revision number")
#assert(#swagbuildnum == 1, "'swag.swg' does not match the compiler build number")

func @print(value: s64);
func @print(value: f64);
func @print(value: [..] u8);
func @assert(value: bool);
func @alloc(size: u32)->*void;
func @realloc(ptr: *void, size: u32)->*void;
func @free(ptr: *void);
func @memcpy(dst: *void, src: *void, size: u32);
func @getcontext()->const *swag.Context;
func @setcontext(context: const *swag.Context);
func @args()->[..] string;

namespace swag
{
	// #########################################################
	// Compiler attributes
	// #########################################################

	attr constexpr() -> "func"
	attr printbc() -> "func"
	attr compiler() -> "func"
	attr inline() -> "func"
    attr macro() -> "func"
    attr mixin() -> "func"
	attr test() -> "func"
	attr foreign(module: string = "", function: string = "") -> "func"
	attr waitsem(ms: s32) -> "func"
	attr pack() -> "struct"
	attr flags() -> "enum"
    attr complete() -> "func"
    attr offset(name: string) -> "var"

	// #########################################################
	// Constants
	// #########################################################

	const s8_min  = -127's8 - 1
	const s16_min = -32767's16 - 1
	const s32_min = -2147483647's32 - 1
	const s64_min = -9223372036854775807's64 - 1

	const s8_max  = 127's8
	const s16_max = 32767's16
	const s32_max = 2147483647's32
	const s64_max = 9223372036854775807's64

	const u8_max  = 0xFF'u8
	const u16_max = 0xFFFF'u16
	const u32_max = 0xFFFF_FFFF'u32
	const u64_max = 0xFFFF_FFFF_FFFF_FFFF'u64

	const f32_min = 1.175494e-38'f32
	const f32_max = 3.402822e+38'f32
	const f64_min = 2.22507e-308'f64
	const f64_max = 1.79769e+308'f64

	// #########################################################
	// Context & allocators
	// #########################################################

	enum AllocatorMode
	{
		Alloc
		Free
		Realloc
	}

    typealias Allocator: func(*AllocatorRequest)->void

	struct AllocatorRequest
	{
		mode:		AllocatorMode = AllocatorMode.Alloc
		size:		u32
		address:	*void
	}

	func defaultAllocator(request: *AllocatorRequest)
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

	struct Context
	{
		allocator: Allocator
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

	struct TypeInfoParam
	{
		base: 			TypeInfo
		namedParam:		string
		pointedType:	const *TypeInfo
		value:			*void
		attributes:		const [..] (string, any)
		offset:			u32
	}

	struct TypeInfoStruct
	{
		base: 		TypeInfo
		fields:		const [..] *TypeInfoParam
		attributes:	const [..] (string, any)
        interfaces: const [..] *TypeInfoParam
	}
    
	struct TypeInfoFunc
	{
		base: 			TypeInfo
		parameters:		const [..] *TypeInfoParam
		returnType:		const *TypeInfo
		attributes:		const [..] (string, any)
	}

	struct TypeInfoEnum
	{
		base: 		TypeInfo
		values:		const [..] *TypeInfoParam
		rawType:	const *TypeInfo
		attributes:	const [..] (string, any)
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
}
