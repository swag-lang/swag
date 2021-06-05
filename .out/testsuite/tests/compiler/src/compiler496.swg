


enum EN : s32
{
	Red = 666;
	Green;
}

//#[Swag.printbc]
#test
{
	using EN
	var val : EN = Red
	@assert(val == 666)
}
