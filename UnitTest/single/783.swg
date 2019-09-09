#module module783

#test
{
	using swag
	
	t := @typeof(u32)
	@assert(t.name == "u32")
	@assert(@countof(t.name) == 3)
	@assert(t.kind == typeinfo_kind.Native)
	
	t1 := @typeof(**bool)
	@assert(t1.name == "**bool")
}