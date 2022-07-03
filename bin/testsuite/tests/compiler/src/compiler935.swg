var x: any = 666

//#[Swag.PrintBc]
#test
{
	t := @kindof(x)
	@assert(t.fullname == "s32")
	ptr := cast(*s32) @dataof(x)
	@assert(ptr[0] == 666)
}
