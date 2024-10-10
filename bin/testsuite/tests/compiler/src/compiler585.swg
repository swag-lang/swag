#global testerror "Err0194"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}