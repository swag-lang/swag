#global testerror "Err0186"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}