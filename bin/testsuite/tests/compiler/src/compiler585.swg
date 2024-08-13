#global testerror "Err0176"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}