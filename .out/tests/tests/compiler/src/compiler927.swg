

//#[Swag.printbc]
func average(x: any)
{
	tt := @kindof(x)
	@assert(tt == @typeof(s32))
}

//#[Swag.printbc]
#test
{
	average(0)
}
