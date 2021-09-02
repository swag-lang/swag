

var x: any = "666"

//#[Swag.PrintBc]
#test
{
	t := @kindof(x)
	@assert(t.name == "string")
	ptr := cast(*string) @dataof(x)
	@assert(ptr[0] == "666")
}
