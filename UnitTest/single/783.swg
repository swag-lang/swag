#module module783

#test
{
	using swag
	
	t := @op(u32, typeof)
	@assert(t.name == "u32")
	@assert(@op(t.name, count) == 3)
	@assert(t.kind == TypeInfoKind.Native)
	
	t1 := @op(**bool, typeof)
	@assert(t1.name == "**bool")
}