using Swag

#[AttrUsage(AttributeUsage.EnumValue)]
attr NC1(x: string);

enum A : f32
{
	#[NC1("TOTO")]
	VAL = 666
}

#test
{
	var ptr = cast(const *TypeInfoEnum) @typeof(A.VAL)
	@assert(@countof(ptr.values) == 1)
	@assert(@countof(ptr.attributes) == 0)
	var ptr1 = cast(const *TypeValue) ptr.values[0]
	var ptrVal = cast(const *f32) ptr1.value
	@assert(dref ptrVal == 666)
}
