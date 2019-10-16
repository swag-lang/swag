


func toto(x: s32)
{
	@assert(x == 666)
}

//#[swag.printbc]
#test
{
	a := 666
	a.toto()
}