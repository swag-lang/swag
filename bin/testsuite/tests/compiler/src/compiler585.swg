#global testerror "Err0189"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}