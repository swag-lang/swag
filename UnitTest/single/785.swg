#module module785

func toto(x: s32)
{
}

//#[swag.printbc]
#test
{
	using swag
	
	t := @typeof(toto(0))
	
	ptr := cast(const *TypeInfoFunc) t
	@assert(@countof(ptr.parameters) == 1)
	
	//@print(cast(s32) ptr.parameters[0].offsetOf)
	
	/*@print(ptr.base.name)
	@print(" ")
	@print(ptr.returnType.name)*/
}