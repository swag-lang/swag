#global testerror "Err0243"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}