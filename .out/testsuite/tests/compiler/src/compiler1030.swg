

//#[Swag.printbc]
#global testerror
#test
{
	x := 10
	init(&x)(0.0)
	@assert(x == 0)
}