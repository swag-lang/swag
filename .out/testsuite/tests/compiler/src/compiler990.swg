using Swag

#[attributeUsage(AttributeUsage.StructVariable)]
attr NC1();

struct Point
{
	#[NC1]
	x: s32

	#[NC1]
	y: s32
}

//#[Swag.printbc]
#test
{
	t := cast(const *TypeInfoStruct) @typeof(Point)
	@assert(@countof(t.fields) == 2)

	loop t.fields
	{
		p := cast(const *TypeInfoParam) t.fields[@index]
		@assert(p.name == (@index == 0 ? "x" : "y"))
		@assert(@countof(p.attributes) == 1)
		@assert(p.attributes[0].item0 == "__compiler990.NC1")
	}
}
