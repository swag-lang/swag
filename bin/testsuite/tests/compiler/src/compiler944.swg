//#[Swag.PrintBc]
func toto(x: f32, y: ...)
{
	@assert(x == 1.0)

	result := 0
	loop @countof(y)
	{
		p := y[@index]
		ptr := cast(*s32) @dataof(p)
		result += dref ptr
	}

	@assert(result == 60)
}

//#[Swag.PrintBc]
#test
{
	toto(1.0, 10, 20, 30)
}
