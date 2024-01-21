#global testerror "Err0303"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}