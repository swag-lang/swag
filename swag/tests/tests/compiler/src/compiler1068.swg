#global testerror

func toString() -> [..] u8
{
	return @[0, 1, 2, 3]
}

//#[swag.printbc]
#test
{
	var x = toString()
	@assert(x[4] == 3)
}