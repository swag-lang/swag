

//#[Swag.PrintBc]
#test
{
	var x: any = 6
	var y = x
	@assert(@kindof(x) == @kindof(y))
	@assert(@typeof(x) == @typeof(y))
	@assert(@dataof(x) == @dataof(y))
}
