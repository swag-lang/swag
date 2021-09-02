alias pair = {string, any}

//#[Swag.PrintBc]
#test
{
	var arr: [..] pair
	var pt: pair

	pt.item0 = "toto"
	pt.item1 = 6
	@assert(@kindof(pt.item1) == @typeof(s32))
}
