

//#[swag.printbc]
func average(x: any)
{
	tt := @kindof(x)
	@assert(tt == @typeof(string))
}

//#[swag.printbc]
#test
{
	average("0")
}
