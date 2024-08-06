#global testerror "Err0223"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}