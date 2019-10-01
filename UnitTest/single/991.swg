#module module991
//#unittest pass semantic
using swag

attr NC1(x: s32) -> var
attr NC2(y: s32) -> var

struct Point
{
	#[NC1(666), NC2(666)]
	x: s32
}

//#[swag.printbc]
#test
{
	t := cast(const *typeinfo_struct) @typeof(Point)
	p := cast(const *typeinfo_param) t.fields[0]
	@assert(p.attributes[0].val0 == "NC1")
	@assert(p.attributes[1].val0 == "NC1.x")
	
	ptr := @kindof(p.attributes[1].val1)
	@assert(ptr.name == "s32")
	ptr1 := cast(*s32) @dataof(p.attributes[1].val1)
	@assert(ptr1[0] == 666)
}
