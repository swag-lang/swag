#module module780

struct TypeInfoNative
{
	kind: s32
}

#test
{
	t := @op(u32, typeof)
	@print(t.kind)
}