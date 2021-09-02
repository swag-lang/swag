


enum EN : s32
{
	Red = 666
	Green
}

//#[Swag.PrintBc]
#test
{
	var val : EN = ?

	using EN
	val = EN.Red
	@assert(val == 666)
	val = Green
	@assert(val == 667)
}
