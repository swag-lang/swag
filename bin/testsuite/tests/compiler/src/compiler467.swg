#global testerror "Err0530"
#test
{
	var a: func() = &F;
	var b: s32 = a();
	@assert(b == 666)
}

func F()->s32
{
	return 666
}
