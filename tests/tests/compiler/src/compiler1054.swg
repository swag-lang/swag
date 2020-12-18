

func(T) delete(addr: *T)
{
}

struct Point
{
	x: s32
}

//#[swag.printbc]
#test
{
	var ptr : *Point
	delete'(Point)(ptr)
}