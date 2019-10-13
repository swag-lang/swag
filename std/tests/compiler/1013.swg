
using swag

attr NC1(x: string) -> enumvalue

enum A
{
	#[NC1("TOTO")]
	VAL
}

#test
{
	ptr := cast(const *TypeinfoEnum) @typeof(A.VAL)
	ptr1 := cast(const *TypeinfoParam) ptr.values[0]
	@assert(ptr1.attributes[0].val0 == "NC1")
	@assert(ptr1.attributes[1].val0 == "NC1.x")
	val := ptr1.attributes[1].val1
	@assert(val is string)
	@assert(val == "TOTO")
}
