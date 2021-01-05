#global testerror

func(op: s32) toto()
{
}

#test
{
	c := 0
	toto!c();
}