

func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}

//#[Swag.printbc]
#test
{
	var x: any = 666
	@assert(valueof's32(x) == 666)
	@assert(valueof(55) == 55)
	@assert(valueof'string("666") == "666")
}
