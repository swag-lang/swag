
#unittest pass semantic

//#[swag.printbc]
#test
{
	var a: ()->s32 = &F;
}

func F()->s32
{
	return 666
}