#global testerror
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}