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

namespace swag 
{
	attr constexpr() -> func;
	attr printbc() -> func;
	attr compiler() -> func;
	attr public() -> func;
	attr foreign(module: string = "") -> func;
	attr semsleep(time: s32) -> func;

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
		VariadicValue
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
	}

	struct TypeInfo
	{
		name:	string
		kind: 	TypeInfoKind = TypeInfoKind.Invalid
		sizeOf: u32
	}

	struct TypeInfoNative
	{
		base: 		TypeInfo
		nativeKind:	TypeInfoNativeKind = TypeInfoNativeKind.Void
	}

	struct TypeInfoPointer
	{
		base: 			TypeInfo
		pointedType:	*TypeInfo
		ptrCount:		u32
	}

	struct TypeInfoParam
	{
		base: 			TypeInfo
		namedParam:		string
		pointedType:	*TypeInfo
		ptrValue:		*void
		offsetOf:		u32
	}

	struct TypeInfoStruct
	{
		base: 		TypeInfo
		fields:		const [..] *TypeInfoParam
	}

	struct TypeInfoFunc
	{
		base: 			TypeInfo
		parameters:		const [..] *TypeInfoParam
		returnType:		*TypeInfo
	}

	struct TypeInfoEnum
	{
		base: 			TypeInfo
		values:			const [..] *TypeInfoParam
		rawType:		*TypeInfo
	}
}
)";
