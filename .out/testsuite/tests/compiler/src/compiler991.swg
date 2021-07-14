using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr NC1(x: s32);
#[AttrUsage(AttributeUsage.StructVariable)]
attr NC2(y: s32);

struct Point
{
	#[NC1(666), NC2(666)]
	x: s32
}

//#[Swag.printbc]
#test
{
	t := cast(const *TypeInfoStruct) @typeof(Point)
	p := cast(const *TypeInfoParam) t.fields[0]

    //@print(cast(s64) @countof(p.attributes))

    @assert(@countof(p.attributes) == 2)
	@assert(p.attributes[0].name == "__compiler991.NC1")
	@assert(p.attributes[1].name == "__compiler991.NC2")

    @assert(@countof(p.attributes[0].params) == 1)
    @assert(@countof(p.attributes[1].params) == 1)

	ptr := @kindof(p.attributes[1].params[0].value)
    //@print(cast(s64) ptr.sizeof)
    //@print(ptr.name)
    //@print("\n")
	@assert(ptr.name == "s32")
	ptr1 := cast(*s32) @dataof(p.attributes[1].params[0].value)
	@assert(ptr1[0] == 666)
}
