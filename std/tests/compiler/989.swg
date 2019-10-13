
//#unittest pass semantic
using swag

attr NC() -> struct
attr NC0() -> struct
attr NC1() -> var

#[NC, NC0]
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
	@assert(@countof(t.attributes) == 2)
	@assert(t.attributes[0].val0 == "NC")
	@assert(t.attributes[1].val0 == "NC0")
}
