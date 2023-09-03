


//#[Swag.PrintBc]
#global testerror "Syn0066"
#test
{
	var a: (s32, bool)->s32 = &F;
}

func F(x: s32, b: s32)->s32
{
	return 666
}