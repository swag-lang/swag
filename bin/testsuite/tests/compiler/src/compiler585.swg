#global testerror "Err0299"
func(op: string) toto()
{
}

#test
{
	var c = 0
	toto'(c)()
}