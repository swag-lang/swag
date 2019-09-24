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

namespace swag 
{
	attr constexpr() -> func;
	attr printbc() -> func;
	attr compiler() -> func;
	attr public() -> func;
	attr foreign(module: string = "") -> func;

	enum typeinfo_kind
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

	enum typeinfo_native_kind
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

	struct typeinfo
	{
		name:	string
		kind: 	typeinfo_kind = typeinfo_kind.Invalid
		sizeof: u32
	}

	struct typeinfo_native
	{
		base: 			typeinfo
		native_kind:	typeinfo_native_kind = typeinfo_native_kind.Void
	}

	struct typeinfo_pointer
	{
		base: 			typeinfo
		pointed_type:	*typeinfo
		ptr_count:		u32
	}

	struct typeinfo_param
	{
		base: 			typeinfo
		named_param:	string
		pointed_type:	*typeinfo
		value:			*void
		offset:			u32
	}

	struct typeinfo_struct
	{
		base: 		typeinfo
		fields:		const [..] *typeinfo_param
	}

	struct typeinfo_func
	{
		base: 			typeinfo
		parameters:		const [..] *typeinfo_param
		return_type:	*typeinfo
	}

	struct typeinfo_enum
	{
		base: 		typeinfo
		values:		const [..] *typeinfo_param
		raw_type:	*typeinfo
	}

	struct typeinfo_variadic
	{
		base: 		typeinfo
	}
}

)";
