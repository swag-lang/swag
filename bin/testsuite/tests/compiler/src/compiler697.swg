#global testerror "Err0306"

func(op: s32) toto()
{
}

#test
{
	var c = 0
	toto'c()
}