#module module733

//#[swag.printbc]
func(T) toto(x: T)
{
	var c: *T;
	c = &x
	@assert(c[0] == "toto")
}
	
#test
{
	cpt := "toto"
	toto(cpt)
}

