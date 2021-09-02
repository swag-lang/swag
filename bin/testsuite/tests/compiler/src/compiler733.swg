

//#[Swag.PrintBc]
func(T) toto(x: T)
{
	var c: const *T;
	c = &x
	@assert(c[0] == "toto")
}

#test
{
	cpt := "toto"
	toto(cpt)
}
