

//#[Swag.PrintBc]
func average(x: any)
{
	tx:= cast(*string) @dataof(x)
	@assert(dref tx == "toto")
}

//#[Swag.PrintBc]
#test
{
	average("toto")
	average("toto")
}
