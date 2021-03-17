


//#[swag.printbc]
#test
{
	a := &F;
	b := a(100, 2);
	@assert(b == 200)
	a = &F1;
	b = a(100, 2);
	@assert(b == 98)
}

func F(x: s32, y: s32)->s32
{
	//@print(x)
	return x * y
}

func F1(x: s32, y: s32)->s32
{
	//@print(x)
	return x - y
}
