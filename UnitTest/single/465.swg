#module module465
#unittest pass semantic

//#[swag.printbytecode]
#test
{
	a := &F;
	b := a();
	//@print(b)
}

func F()->s32
{
	return 666
}