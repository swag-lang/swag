var x: any = 666

//#[swag.printbc]
#test
{
	t := @kindof(x)
	@assert(t.name == "s32")
	ptr := cast(*s32) @dataof(x)
	@assert(ptr[0] == 666)
}
