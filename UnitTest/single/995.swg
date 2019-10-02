#module module995
using swag

attr NC1(x: string) -> enumvalue

enum A
{
	#[NC1("toto")]
	VAL
}

#test
{
	ptr := cast(const *typeinfo_param) @typeof(A.VAL)
	@assert(@countof(ptr.attributes) == 2)
	@assert(ptr.attributes[0].val0 == "NC1")
	@assert(ptr.attributes[1].val0 == "NC1.x")
	val := ptr.attributes[1].val1
	@assert(@kindof(val) == @typeof(string))
	ptrVal := cast(*string) @dataof(val);
	@assert(ptrVal[0] == "toto")
}
