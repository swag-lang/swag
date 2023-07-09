

//#[Swag.PrintBc]
func toto(x: any)
{
	var type = @kindof(x)
	@assert(type.fullname == "u8")
	var ptr = cast(*u8) @dataof(x)
	//@print(cast(s32) ptr[0])
	@assert(dref ptr == 125)
}


//#[Swag.PrintBc]
#test
{
	var ptr : [] u8 = [125, 2, 4]
	toto(ptr[0])
}