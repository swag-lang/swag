#global testerror "Err0699"
func(T) toto(x: *T)
{
	@print(t[0])
}

#test
{
	var cpt = 50
	toto's32(&cpt)
}
