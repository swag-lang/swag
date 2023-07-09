
//#[Swag.PrintBc]
#test
{
	using Swag

	var t = @typeof(u32)
	@assert(t.fullname == "u32")
	@assert(@countof(t.fullname) == 3)
	@assert(t.kind == TypeInfoKind.Native)

	var t1 = @typeof(**bool)
	@assert(t1.fullname == "**bool")
}