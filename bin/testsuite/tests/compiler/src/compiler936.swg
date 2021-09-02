

var x: any = 666.0'f64

//#[Swag.PrintBc]
#test
{
	t := @kindof(x)
	@assert(t.name == "f64")
	ptr := cast(*f64) @dataof(x)
	@assert(ptr[0] == 666)
}
