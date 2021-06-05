struct Point
{
	x: s32
}

//#[Swag.printbc]
#test
{
	pair := @{"toto", Point{10}}
	@assert(pair.item1.x == 10)
}
