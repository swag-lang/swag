#global testerror "Err0188"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}