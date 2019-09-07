#module module780

namespace Swag
{
struct TypeInfo
{
	kind: s32
}

struct TypeInfoNative
{
	base: TypeInfo
}
}

#test
{
	t := @op(u32, typeof)
	@print(t.kind)
}