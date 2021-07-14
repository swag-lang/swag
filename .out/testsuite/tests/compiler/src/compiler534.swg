


func toto(x: s32)
{
	@assert(x == 666)
}

//#[Swag.PrintBc]
#test
{
	a := 666
	a.toto()
}