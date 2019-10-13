#module module463
#unittest pass semantic

//#[swag.printbc]
#test
{
	var a: (s32, bool)->s32 = &F;
}

func F(x: s32, b: bool)->s32
{
	return 666
}