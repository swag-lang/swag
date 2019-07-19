#module module462
#unittest pass semantic

//#[swag.printbytecode]
#test
{
	var a: ()->s32 = &F;
}

func F()->s32
{
	return 666
}