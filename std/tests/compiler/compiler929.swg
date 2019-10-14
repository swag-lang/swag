

//#[swag.printbc]
func average(x: any)
{
	tt := @kindof(x)
	@assert(tt.name == "s32")
	
	using swag.TypeinfoKind
	@assert(tt.kind == Native)
	@assert(tt.sizeof == 4)
}

//#[swag.printbc]
#test
{
	average(666)
}
