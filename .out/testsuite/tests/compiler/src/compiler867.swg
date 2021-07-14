

var G: s32 = 0

//#[Swag.PrintBc]
#test
{
	G = 0

	func myFunc(val: s32, x: func(s32)->void)
	{
		x(val);
	}

	myFunc(10, @(x: s32) { G = x; });
	@assert(G == 10)
	myFunc(10, @(x: s32) { G = x + 1; });
	@assert(G == 11)
}