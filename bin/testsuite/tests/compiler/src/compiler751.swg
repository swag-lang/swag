

struct Point
{
	x: s32
	y: s32
}

func toto()->s32
{
	//var pt: Point
	return 666
}

//#[Swag.PrintBc]
#test
{
	var x = &toto
	var c = x()
	@assert(c == 666)
}
