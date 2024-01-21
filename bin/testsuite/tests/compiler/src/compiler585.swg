#global testerror "Err0304"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}