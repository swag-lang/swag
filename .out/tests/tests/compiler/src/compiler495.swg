


enum EN : s32
{
	Red = 666;
	Green;
}

//#[swag.printbc]
#test
{
	using EN
	val := Red
	@assert(val == 666)
}
