// swag compiler mandatory file
#assert(#swagversion == 0, "'swag.swg' does not match the compiler version number")
#assert(#swagrevision == 0, "'swag.swg' does not match the compiler revision number")
#assert(#swagbuildnum == 1, "'swag.swg' does not match the compiler build number")

func @print(value: s64);
func @print(value: f64);
func @print(value: string);
func @assert(value: bool);
func @alloc(size: u32)->*void;
func @realloc(ptr: *void, size: u32)->*void;
func @free(ptr: *void);
func @memcpy(dst: *void, src: *void, size: u32);
func @context()->*swag.Context;
func @args()->[..] string;

namespace swag 
{
	// #########################################################
	// Compiler attributes
	// #########################################################

	attr constexpr() -> func
	attr printbc() -> func
	attr compiler() -> func
	attr public() -> func
	attr test() -> func
	attr foreign(module: string = "", generated: bool = false) -> func
	attr waitsem(ms: s32) -> func

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
		allocator: (*AllocatorRequest)->void
	}

	// #########################################################
	// Type reflection
	// #########################################################
	
	enum TypeinfoKind
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

	enum TypeinfoNativeKind
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

	struct Typeinfo
	{
		name:	string
		kind: 	TypeinfoKind = ?
		sizeof: u32
	}

	struct TypeinfoNative
	{
		base: 			Typeinfo
		nativeKind:		TypeinfoNativeKind = TypeinfoNativeKind.Void
	}

	struct TypeinfoPointer
	{
		base: 			Typeinfo
		finalType:		const *Typeinfo
		pointedType:	const *Typeinfo
		ptrCount:		u32
	}

	struct TypeinfoParam
	{
		base: 			Typeinfo
		namedParam:		string
		pointedType:	const *Typeinfo
		value:			*void
		attributes:		const [..] { string, any }
		offset:			u32
	}

	struct TypeinfoStruct
	{
		base: 		Typeinfo
		fields:		const [..] *TypeinfoParam
		attributes:	const [..] { string, any }
	}

	struct TypeinfoFunc
	{
		base: 			Typeinfo
		parameters:		const [..] *TypeinfoParam
		returnType:		const *Typeinfo
		attributes:		const [..] { string, any }
	}

	struct TypeinfoEnum
	{
		base: 		Typeinfo
		values:		const [..] *TypeinfoParam
		rawType:	const *Typeinfo
		attributes:	const [..] { string, any }
	}

	struct TypeinfoVariadic
	{
		base:	Typeinfo
	}

	struct TypeinfoArray
	{
		base: 			Typeinfo
		pointedType:	const *Typeinfo
		finalType:		const *Typeinfo
		count:			u32
		totalCount:		u32
	}

	struct TypeinfoSlice
	{
		base: 			Typeinfo
		pointedType:	const *Typeinfo
	}
}