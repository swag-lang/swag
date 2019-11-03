
using swag

attr NC1(x: string) -> "enumvalue"

enum A
{
	#[NC1("TOTO")]
	VAL
}

#test
{
	ptr := cast(const *TypeinfoEnum) @typeof(A.VAL)
	@assert(@countof(ptr.values) == 1)
	@assert(@countof(ptr.attributes) == 0)
	ptr1 := cast(const *TypeinfoParam) ptr.values[0]
	@assert(@countof(ptr1.attributes) == 2)
	@assert(ptr1.attributes[0].val0 == "compiler995.NC1")
	@assert(ptr1.attributes[1].val0 == "compiler995.NC1.x")
	val := ptr1.attributes[1].val1
	@assert(@kindof(val) == @typeof(string))
	ptrVal := cast(*string) @dataof(val)
	@assert(ptrVal[0] == "TOTO")
}
