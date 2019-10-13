#module module869

var G: s32 = 0

//#[swag.printbc]
#unittest error
#test
{
	G = 0
	
	func myFunc(val: s32, x: (s32)->s32)
	{ 
		x(val); 
	}
	
	myFunc(10, func(x: s32)->s32 { G = x; });
	@assert(G == 10)
}