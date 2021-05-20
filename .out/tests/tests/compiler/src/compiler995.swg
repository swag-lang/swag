using Swag

#[attributeUsage(AttributeUsage.EnumValue)]
attr NC1(x: string);

enum A
{
	#[NC1("TOTO")]
	VAL
}

#test
{
	ptr := cast(const *TypeInfoEnum) @typeof(A.VAL)
	@assert(@countof(ptr.values) == 1)
	@assert(@countof(ptr.attributes) == 0)

	ptr1 := cast(const *TypeInfoParam) ptr.values[0]
	@assert(@countof(ptr1.attributes) == 1)
	@assert(ptr1.attributes[0].name == "__compiler995.NC1")
	@assert(ptr1.attributes[0].params[0].name == "x")

	val := ptr1.attributes[0].params[0].value
	@assert(@kindof(val) == @typeof(string))
	ptrVal := cast(*string) @dataof(val)
	@assert(ptrVal[0] == "TOTO")
}
