#global testerror

var G: s32 = 0

//#[Swag.printbc]
#test
{
	G = 0

	func myFunc(val: s32, x: func(s32)->s32)
	{
		discard x(val);
	}

	myFunc(10, @(x: s32) { G = x; });
	@assert(G == 10)
}