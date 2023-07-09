var x: any = 666

//#[Swag.PrintBc]
#test
{
	var t = @kindof(x)
	@assert(t.fullname == "s32")
	var ptr = cast(*s32) @dataof(x)
	@assert(dref ptr == 666)
}
