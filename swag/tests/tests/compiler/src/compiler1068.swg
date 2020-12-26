

func toString() -> [..] u8
{
	return @[0, 1, 2, 3]
}

#unittest error
//#[swag.printbc]
#test
{
	var x = toString()
	@assert(x[4] == 3)
}