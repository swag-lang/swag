using Swag

#[AttrUsage(AttributeUsage.EnumValue)]
attr NC1(x: string);

enum A
{
	#[NC1("TOTO")]
	VAL
}

#test
{
	var ptr = cast(const *TypeInfoEnum) @typeof(A.VAL)
	var ptr1 = cast(const *TypeValue) ptr.values[0]

	//@print(ptr1.attributes[0].item0)
	@assert(ptr1.attributes[0].type == @typeof(NC1))
	@assert(ptr1.attributes[0].params[0].name == "x")

	var val = ptr1.attributes[0].params[0].value
	@assert(@kindof(val) == string)
	@assert(val == "TOTO")
}
