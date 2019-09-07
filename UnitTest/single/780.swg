#module module780

namespace Swag
{
	enum TypeInfoKind
	{
		Invalid
		Native
	}

	enum TypeInfoNativeKind
	{
		Void
		NativeU32
	}

	struct TypeInfo
	{
		kind: TypeInfoKind = TypeInfoKind.Invalid
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
}