

func(T) delete(addr: *T)
{
}

struct Point
{
	x: s32
}

//#[Swag.printbc]
#test
{
	var ptr : *Point
	delete'(Point)(ptr)
}