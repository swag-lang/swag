#module module785

func toto(x: s32)
{
}

//#[swag.printbc]
#test
{
	using swag
	
	t := @typeof(toto(0))
	
	ptr := cast(const *typeinfo_func) t
	@assert(@countof(ptr.parameters) == 1)
	@assert(ptr.parameters[0].named_param == "x")
	
	/*@print(ptr.base.name)
	@print(" ")
	@print(ptr.returnType.name)*/
}