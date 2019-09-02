#module module751

struct Point
{
	var x: s32
	var y: s32
}

func toto()->s32
{
	//var pt: Point
	return 666
}

//#[swag.printbc]
#test
{
	x := &toto
	c := x()
	@assert(c == 666)
}
