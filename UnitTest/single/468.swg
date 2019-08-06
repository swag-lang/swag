#module module468
#unittest pass semantic

//#[swag.printbc]
#test
{
	var a: ()->s32 = &F;
	var b: s32 = a();
	@assert(b == 666)
}

func F()->s32
{
	return 666
}
