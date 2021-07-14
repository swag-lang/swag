


var c: s32 = 0

//#[Swag.PrintBc]
func(op: string) toto()
{
	if(op == "toto")
		c = 1
	if(op == "toto1")
		c = 2
}

//#[Swag.PrintBc]
#test
{
	toto'("toto")()
	@assert(c == 1)
	toto'("toto1")()
	@assert(c == 2)
}