#module module496
#unittest pass semantic

enum EN : s32
{
	Red = 666;
	Green;
}

//#[swag.printbytecode]
#test
{
	using EN
	var val : EN = Red
	@assert(val == 666)
}
