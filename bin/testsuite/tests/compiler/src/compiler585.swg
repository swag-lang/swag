#global testerror "Err0178"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}