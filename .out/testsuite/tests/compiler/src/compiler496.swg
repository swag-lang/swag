


enum EN : s32
{
	Red = 666;
	Green;
}

//#[Swag.PrintBc]
#test
{
	using EN
	var val : EN = Red
	@assert(val == 666)
}
