using Swag

#[AttrUsage(AttributeUsage.Struct)]
attr NC();
#[AttrUsage(AttributeUsage.Struct)]
attr NC0();
#[AttrUsage(AttributeUsage.StructVariable)]
attr NC1();

#[NC, NC0]
struct Point
{
	#[NC1]
	x: s32

	#[NC1]
	y: s32
}

//#[Swag.PrintBc]
#test
{
	t := cast(const *TypeInfoStruct) @typeof(Point)
	@assert(@countof(t.attributes) == 2)
	@assert(t.attributes[0].item0 == "__compiler989.NC")
	@assert(t.attributes[1].item0 == "__compiler989.NC0")
}
