

//#[swag.printbc]
func average(x: any)
{
	tt := @kindof(x)
	@assert(tt == @typeof(s32))
}

//#[swag.printbc]
#test
{
	average(0)
}
