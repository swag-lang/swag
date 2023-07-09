

//#[Swag.PrintBc]
func average(x: any)
{
	var tt = @kindof(x)
	@assert(tt == @typeof(s32))
}

//#[Swag.PrintBc]
#test
{
	average(0)
}
