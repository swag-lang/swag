

//#[Swag.printbc]
func average(x: any)
{
	tt := @kindof(x)
	@assert(tt.name == "s32")

	using Swag.TypeInfoKind
	@assert(tt.kind == Native)
	@assert(tt.sizeof == 4)
}

//#[Swag.printbc]
#test
{
	average(666)
}
