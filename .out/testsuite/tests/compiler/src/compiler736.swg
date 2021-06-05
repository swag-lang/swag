

//#[Swag.printbc]

func(T) toto(x: *T)
{
	@assert(x[0] == 666)
}

#test
{
	cpt := 666
	toto(&cpt)
}
