

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
	x := &toto
	c := x()
	@assert(c == 666)
}
