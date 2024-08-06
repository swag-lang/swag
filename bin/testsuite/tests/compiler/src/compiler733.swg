#global testerror "Err0153"

//#[Swag.PrintBc]
func(T) toto(x: T)
{
	var c: const *T
	c = &x
	@assert(dref c == "toto")
}

#test
{
	let cpt = "toto"
	toto(cpt)
}
