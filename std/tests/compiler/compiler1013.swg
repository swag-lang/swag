
using swag

attr NC1(x: string) -> "enumvalue"

enum A
{
	#[NC1("TOTO")]
	VAL
}

#test
{
	ptr := cast(const *TypeInfoEnum) @typeof(A.VAL)
	ptr1 := cast(const *TypeInfoParam) ptr.values[0]
	//@print(ptr1.attributes[0].item0)
	@assert(ptr1.attributes[0].item0 == "compiler1013.NC1")
	@assert(ptr1.attributes[1].item0 == "compiler1013.NC1.x")
	val := ptr1.attributes[1].item1
	@assert(val is string)
	@assert(val == "TOTO")
}
