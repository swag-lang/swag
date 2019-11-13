

using swag

attr NC() -> "struct"
attr NC0() -> "struct"
attr NC1() -> "var"

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
	@assert(t.attributes[0].item0 == "compiler989.NC")
	@assert(t.attributes[1].item0 == "compiler989.NC0")
}
