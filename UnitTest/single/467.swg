#module module467
#unittest pass semantic

//#[swag.printbytecode]
#unittest error
#test
{
	var a: () = &F;
	var b: s32 = a();
	@assert(b == 666)
}

func F()->s32
{
	return 666
}
