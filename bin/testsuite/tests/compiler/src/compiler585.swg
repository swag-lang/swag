#global testerror "Err0815"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}