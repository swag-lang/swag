


enum EN : s32
{
	Red = 666;
	Green;
}

//#[Swag.printbc]
#test
{
	using EN
	val := Red
	@assert(val == 666)
}
