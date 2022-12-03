#global testerror

//#[Swag.PrintBc]
func(T) toto(x: T)
{
	var c: const *T
	c = &x
	@assert(dref c == "toto")
}

#test
{
	cpt := "toto"
	toto(cpt)
}
