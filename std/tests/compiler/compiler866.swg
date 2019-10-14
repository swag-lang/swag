

//#[swag.printbc]
#test
{
	func a(x: s32)->s32 
	{ 
		return x; 
	}
	
	b := a(3)
	@assert(b == 3)
}