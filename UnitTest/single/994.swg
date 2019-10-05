#module module994
using swag

attr NC1(x: string) -> enum

#[NC1("toto")]
enum A
{
}

#test
{
	ptr := cast(const *TypeinfoEnum) @typeof(A)
	@assert(@countof(ptr.attributes) == 2)
	@assert(ptr.attributes[0].val0 == "NC1")
	@assert(ptr.attributes[1].val0 == "NC1.x")
	val := ptr.attributes[1].val1
	@assert(@kindof(val) == @typeof(string))
	ptrVal := cast(*string) @dataof(val);
	@assert(ptrVal[0] == "toto")
}
