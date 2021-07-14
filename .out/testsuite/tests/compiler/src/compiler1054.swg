

func(T) delete(addr: *T)
{
}

struct Point
{
	x: s32
}

//#[Swag.PrintBc]
#test
{
	var ptr : *Point
	delete'(Point)(ptr)
}