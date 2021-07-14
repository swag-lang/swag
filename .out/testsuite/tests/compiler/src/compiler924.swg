

//#[Swag.PrintBc]
func average(x: any)
{
	tx:= cast(*string) @dataof(x)
	@assert(tx[0] == "toto")
}

//#[Swag.PrintBc]
#test
{
	average("toto")
	average("toto")
}
