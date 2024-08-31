#global testerror "Err0216"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}