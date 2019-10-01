#module module990
//#unittest pass semantic
using swag

attr NC1() -> var

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
	t := cast(const *typeinfo_struct) @typeof(Point)
	@assert(@countof(t.fields) == 2)
	
	loop t.fields
	{
		p := cast(const *typeinfo_param) t.fields[index]
		@assert(p.named_param == (index == 0 ? "x" : "y"))
		@assert(@countof(p.attributes) == 1)
		@assert(p.attributes[0].val0 == "NC1")
	}
}
