


//#[Swag.PrintBc]
#test
{
	var a = &F;
	var b = a(100);
	@assert(b == 100)
}

func F(x: s32)->s32
{
	return x
}
