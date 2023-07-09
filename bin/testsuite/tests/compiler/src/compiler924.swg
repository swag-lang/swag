

//#[Swag.PrintBc]
func average(x: any)
{
	var tx = cast(*string) @dataof(x)
	@assert(dref tx == "toto")
}

//#[Swag.PrintBc]
#test
{
	average("toto")
	average("toto")
}
