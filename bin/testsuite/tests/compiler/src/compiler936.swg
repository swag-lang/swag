

var x: any = 666.0'f64

//#[Swag.PrintBc]
#test
{
	t := @kindof(x)
	@assert(t.fullname == "f64")
	ptr := cast(*f64) @dataof(x)
	@assert(dref ptr == 666)
}
