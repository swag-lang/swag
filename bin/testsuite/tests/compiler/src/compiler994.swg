using Swag

#[AttrUsage(AttributeUsage.Enum)]
attr NC1(x: string);

#[NC1("toto")]
enum A
{
}

#test
{
	var ptr = cast(const *TypeInfoEnum) @typeof(A)
	@assert(@countof(ptr.attributes) == 1)
	@assert(ptr.attributes[0].type == @typeof(NC1))
	@assert(ptr.attributes[0].params[0].name  == "x")

	var val = ptr.attributes[0].params[0].value
	@assert(@kindof(val) == @typeof(string))
	var ptrVal = cast(*string) @dataof(val);
	@assert(dref ptrVal == "toto")
}
