#global testerror "Err0244"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}