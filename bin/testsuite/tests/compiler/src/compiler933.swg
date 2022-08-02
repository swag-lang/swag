

/*func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}*/

func valueof(x: any)
{
	ptr := cast(*s32) @dataof(x)
	@assert(dref ptr == 666)
}

//#[Swag.PrintBc]
#test
{
	var x: any = 666
	valueof(x)
}
