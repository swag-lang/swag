#global testerror "Err0221"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}