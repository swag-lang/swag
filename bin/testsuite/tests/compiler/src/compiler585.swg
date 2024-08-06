#global testerror "Err0293"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}