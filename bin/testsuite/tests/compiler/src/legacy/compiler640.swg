


struct(T) Point
{
	x: T = 1
}

func(op: string) toto()
{
}

//#[Swag.PrintBc]
#test
{
	var pt: Point's32
	@assert(pt.x == 1)

	toto'"toto"();
}
