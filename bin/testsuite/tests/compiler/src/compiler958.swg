

//#[Swag.PrintBc]
func toto(x: any)
{
	type := @kindof(x)
	@assert(type.name == "u8")
	ptr := cast(*u8) @dataof(x)
	//@print(cast(s32) ptr[0])
	@assert(ptr[0] == 125)
}


//#[Swag.PrintBc]
#test
{
	var ptr : [] u8 = @[125, 2, 4]
	toto(ptr[0])
}