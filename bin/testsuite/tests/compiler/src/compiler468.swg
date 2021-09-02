


//#[Swag.PrintBc]
#test
{
	var a: func()->s32 = &F;
	var b: s32 = a();
	@assert(b == 666)
}

func F()->s32
{
	return 666
}
