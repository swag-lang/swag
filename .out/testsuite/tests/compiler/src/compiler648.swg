

var gx: s32

func(T) toto()
{
	var x: T = 100
	gx = x
}

//#[Swag.printbc]
//#global testerror
#test
{
	toto's32()
	@assert(gx == 100)
}
