#module module958

//#[swag.printbc]
func toto(x: any)
{
	type := @kindof(x)
	@assert(type.name == "u8")
	ptr := cast(*u8) @dataof(x)
	@assert(ptr[0] == 125)
}


//#[swag.printbc]
#test
{
	var ptr : [] u8 = [125, 2, 4]
	toto(ptr[0])
}