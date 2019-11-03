

using swag

attr NC1() -> "var"

struct Point
{
	#[NC1]
	x: s32
	
	#[NC1]
	y: s32
}

//#[swag.printbc]
#test
{
	t := cast(const *TypeinfoStruct) @typeof(Point)
	@assert(@countof(t.fields) == 2)
	
	loop t.fields
	{
		p := cast(const *TypeinfoParam) t.fields[index]
		@assert(p.namedParam == (index == 0 ? "x" : "y"))
		@assert(@countof(p.attributes) == 1)
		@assert(p.attributes[0].val0 == "compiler990.NC1")
	}
}
