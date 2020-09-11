using swag

#[attributeUsage(AttributeUsage.Enum)]
attr NC1(x: string);

#[NC1("toto")]
enum A
{
}

#test
{
	ptr := cast(const *TypeInfoEnum) @typeof(A)
	@assert(@countof(ptr.attributes) == 1)
	@assert(ptr.attributes[0].name == "__compiler994.NC1")
	@assert(ptr.attributes[0].params[0].name  == "x")

	val := ptr.attributes[0].params[0].value
	@assert(@kindof(val) == @typeof(string))
	ptrVal := cast(*string) @dataof(val);
	@assert(ptrVal[0] == "toto")
}
