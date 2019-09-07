#module module780

struct TypeInfo
{
	kind: s32
}

struct TypeInfoNative
{
	base: TypeInfo
}

#test
{
	t := @op(u32, typeof)
	@print(t.kind)
}