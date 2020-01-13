using swag

#[attributeUsage(AttributeUsage.Field)]
attr NC1(x: s32);
#[attributeUsage(AttributeUsage.Field)]
attr NC2(y: s32);

struct Point
{
	#[NC1(666), NC2(666)]
	x: s32
}

//#[swag.printbc]
#test
{
	t := cast(const *TypeInfoStruct) @typeof(Point)
	p := cast(const *TypeInfoParam) t.fields[0]
	@assert(p.attributes[0].item0 == "compiler991.NC1")
	@assert(p.attributes[1].item0 == "compiler991.NC1.x")
	
	ptr := @kindof(p.attributes[1].item1)
	@assert(ptr.name == "s32")
	ptr1 := cast(*s32) @dataof(p.attributes[1].item1)
	@assert(ptr1[0] == 666)
}
