#module "compiler467"


//#[swag.printbc]
#unittest error
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
