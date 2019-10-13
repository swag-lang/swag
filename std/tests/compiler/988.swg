
//#unittest pass semantic

typealias pair: {string, any}

//#[swag.printbc]
#test
{
	var arr: [..] pair
	var pt: pair
	
	pt.val0 = "toto"
	pt.val1 = 6
	@assert(@kindof(pt.val1) == @typeof(s32))
}
