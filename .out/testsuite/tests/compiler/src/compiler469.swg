


//#[Swag.PrintBc]
#test
{
	a := &F;
	b := a(100);
	@assert(b == 100)
}

func F(x: s32)->s32
{
	return x
}
