#global testerror "Err0530"

//#[Swag.PrintBc]
#test
{
	var a = &F;
	a = &F1;
}

func F(x: s32, y: s32)->s32
{
	//@print(x)
	return x * y
}

func F1(x: s32)->s32
{
	//@print(x)
	return x
}
