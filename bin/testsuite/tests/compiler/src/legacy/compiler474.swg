


//#[Swag.PrintBc]
#global testerror "Err0264"
#test
{
	var a: (s32, bool)->s32 = &F;
}

func F(x: s32, b: s32)->s32
{
	return 666
}