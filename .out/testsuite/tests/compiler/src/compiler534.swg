


func toto(x: s32)
{
	@assert(x == 666)
}

//#[Swag.printbc]
#test
{
	a := 666
	a.toto()
}