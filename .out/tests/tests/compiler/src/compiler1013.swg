using swag

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
	ptr1 := cast(const *TypeInfoParam) ptr.values[0]

	//@print(ptr1.attributes[0].item0)
	@assert(ptr1.attributes[0].name == "__compiler1013.NC1")
	@assert(ptr1.attributes[0].params[0].name == "x")

	val := ptr1.attributes[0].params[0].value
	@assert(@kindof(val) == string)
	@assert(val == "TOTO")
}
