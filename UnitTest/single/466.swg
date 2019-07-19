#module module466
#unittest pass semantic

//#[swag.printbytecode]
#test
{
	a := &F;
	var b: s32 = a();
}

func F()->s32
{
	return 666
}