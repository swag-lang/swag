

var x: any = "666"

//#[Swag.PrintBc]
#test
{
	var t = @kindof(x)
	@assert(t.fullname == "string")
	var ptr = cast(*string) @dataof(x)
	@assert(dref ptr == "666")
}
