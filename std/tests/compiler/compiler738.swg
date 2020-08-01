

//#[swag.printbc]

func(T) toto(x: const [..] T)
{
	@assert(x[0] == 1)
	@assert(x[1] == 2)
	@assert(x[2] == 3)
	@assert(x[3] == 4)
}

#test
{
	var cpt: const [..] s32 = @[1,2,3,4]
	toto(cpt)
}
