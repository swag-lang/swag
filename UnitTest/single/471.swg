#module module471
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	a := &F;
	b := a(100, 2);
	@assert(b == 200)
}

func F(x: s32, y: s32)->s32
{
	//@print(x)
	return x * y
}
