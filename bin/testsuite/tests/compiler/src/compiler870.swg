

var G: s32 = 0

//#[Swag.PrintBc]
#test
{
	func myFunc(val: s32, x: func(s32))
	{
		x(val);
	}

	func tt(val: s32)
	{
		G = val
	}

	G = 0
	myFunc(10, func(x: s32) { G = x; });
	@assert(G == 10)

	G = 0
	myFunc(10, &tt);
	@assert(G == 10)
}