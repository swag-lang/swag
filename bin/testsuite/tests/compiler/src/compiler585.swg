#global testerror "Err0306"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}