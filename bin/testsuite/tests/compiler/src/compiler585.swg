#global testerror "Err0219"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}