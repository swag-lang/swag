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
	@assert(@countof(ptr.attributes) == 2)
	@assert(ptr.attributes[0].item0 == "compiler994.NC1")
	@assert(ptr.attributes[1].item0 == "compiler994.NC1.x")
	val := ptr.attributes[1].item1
	@assert(@kindof(val) == @typeof(string))
	ptrVal := cast(*string) @dataof(val);
	@assert(ptrVal[0] == "toto")
}
