
using swag

attr NC1(x: string) -> enumvalue

enum A : f32
{
	#[NC1("TOTO")]
	VAL = 666
}

#test
{
	ptr := cast(const *TypeinfoEnum) @typeof(A.VAL)
	@assert(@countof(ptr.values) == 1)
	@assert(@countof(ptr.attributes) == 0)
	ptr1 := cast(const *TypeinfoParam) ptr.values[0]
	ptrVal := cast(*f32) ptr1.value
	@assert(ptrVal[0] == 666)
}
