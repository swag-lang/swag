
//#[Swag.PrintBc]
#test
{
	using Swag

	t := @typeof(u32)
	@assert(t.fullname == "u32")
	@assert(@countof(t.fullname) == 3)
	@assert(t.kind == TypeInfoKind.Native)

	t1 := @typeof(**bool)
	@assert(t1.fullname == "**bool")
}