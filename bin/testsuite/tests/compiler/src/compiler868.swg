#global testerror "Err0549"

var G: s32 = 0

//#[Swag.PrintBc]
#test
{
	G = 0

	func myFunc(val: s32, x: func(s32)->s32)
	{
		discard x(val);
	}

	myFunc(10, func(x: s32) { G = x; });
	@assert(G == 10)
}