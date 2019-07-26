#module module495
//#unittest pass semantic

enum EN : s32
{
	Red = 666;
	Green;
}

//#[swag.printbytecode]
#test
{
	using EN
	val := Red
	@assert(val == 666)
}
