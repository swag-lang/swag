#module module780

namespace Swag
{
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
		kind: 	TypeInfoKind = TypeInfoKind.Invalid
		sizeOf: u32
	}

	struct TypeInfoNative
	{
		base: 		TypeInfo
		nativeKind:	TypeInfoNativeKind = TypeInfoNativeKind.Void
	}
}

#test
{
	t := @op(u32, typeof)
	@print(cast(s32) t.kind)
	@print(cast(s32) t.sizeOf)
	
	ptr := cast(const *Swag.TypeInfoNative) t;
	@print(cast(s32) ptr.nativeKind)
}