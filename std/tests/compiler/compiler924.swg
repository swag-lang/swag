

//#[swag.printbc]
func average(x: any)
{
	tx:= cast(*string) @dataof(x)
	@assert(tx[0] == "toto")
}

//#[swag.printbc]
#test
{
	average("toto")
	average("toto")
}
