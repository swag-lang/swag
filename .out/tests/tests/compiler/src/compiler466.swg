


//#[Swag.printbc]
#test
{
	a := &F;
	var b: s32 = a();
	@assert(b == 666)
}

func F()->s32
{
	return 666
}

func F1()->s32
{
	return 667
}