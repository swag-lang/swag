#global testerror

func toString() -> [..] u8
{
	return @[0, 1, 2, 3]
}

//#[Swag.printbc]
#test
{
	var x = toString()
	@assert(x[4] == 3)
}