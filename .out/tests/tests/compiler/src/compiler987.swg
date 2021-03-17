struct Point
{
	x: s32
}

//#[swag.printbc]
#test
{
	pair := @{"toto", Point{10}}
	@assert(pair.item1.x == 10)
}
