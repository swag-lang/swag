using Swag

#[attributeUsage(AttributeUsage.EnumValue)]
attr NC1(x: string);

enum A : f32
{
	#[NC1("TOTO")]
	VAL = 666
}

#test
{
	ptr := cast(const *TypeInfoEnum) @typeof(A.VAL)
	@assert(@countof(ptr.values) == 1)
	@assert(@countof(ptr.attributes) == 0)
	ptr1 := cast(const *TypeInfoParam) ptr.values[0]
	ptrVal := cast(const *f32) ptr1.value
	@assert(ptrVal[0] == 666)
}
