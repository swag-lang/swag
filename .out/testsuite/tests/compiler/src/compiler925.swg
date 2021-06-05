

//#[Swag.printbc]
func average(x: any)
{
	tt := @kindof(x)
	@assert(tt == @typeof(string))
}

//#[Swag.printbc]
#test
{
	average("0")
}
