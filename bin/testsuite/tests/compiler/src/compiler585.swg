#global testerror "Err0215"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}