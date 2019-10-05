const char* g_Runtime = R"(

func @print(value: s32);
func @print(value: s64);
func @print(value: f32);
func @print(value: f64);
func @print(value: char);
func @print(value: string);
func @assert(value: bool);
func @alloc(size: u32)->*void;
func @free(ptr: *void);
func @memcpy(dst: *void, src: *void, size: u32);
func @getcontext()->*swag.context;

namespace swag 
{
	attr constexpr() -> func
	attr printbc() -> func
	attr compiler() -> func
	attr public() -> func
	attr test() -> func
	attr foreign(module: string = "") -> func
	attr waitsem(ms: s32) -> func

	func defaultAllocator(size: string)
	{
		@print(size)
	}

	struct context
	{
		allocator: (string)->void
	}

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
		kind: 	TypeinfoKind = TypeinfoKind.Invalid
		sizeof: u32
	}

	struct TypeinfoNative
	{
		base: 			Typeinfo
		native_kind:	TypeinfoNativeKind = TypeinfoNativeKind.Void
	}

	struct TypeinfoPointer
	{
		base: 			Typeinfo
		pointed_type:	const *Typeinfo
		ptr_count:		u32
	}

	struct TypeinfoParam
	{
		base: 			Typeinfo
		named_param:	string
		pointed_type:	const *Typeinfo
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
		return_type:	const *Typeinfo
		attributes:		const [..] { string, any }
	}

	struct TypeinfoEnum
	{
		base: 		Typeinfo
		values:		const [..] *TypeinfoParam
		raw_type:	const *Typeinfo
		attributes:	const [..] { string, any }
	}

	struct TypeinfoVariadic
	{
		base:	Typeinfo
	}

	struct TypeinfoArray
	{
		base: 			Typeinfo
		pointed_type:	const *Typeinfo
		final_type:		const *Typeinfo
		count:			u32
		totalCount:		u32
	}

	struct TypeinfoSlice
	{
		base: 			Typeinfo
		pointed_type:	const *Typeinfo
	}
}

)";
