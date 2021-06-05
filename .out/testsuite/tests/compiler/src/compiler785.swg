

//#[Swag.waitsem(100)]
func toto(x: s32)
{
}

//#[Swag.printbc]
#test
{
	using Swag

	t := @typeof(toto)

	ptr := cast(const *TypeInfoFunc) t
	@assert(@countof(ptr.parameters) == 1)
	@assert(ptr.parameters[0].name == "x")

	/*@print(ptr.base.name)
	@print(" ")
	@print(ptr.returnType.name)*/
}