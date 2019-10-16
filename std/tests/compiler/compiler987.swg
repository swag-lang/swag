


struct Point
{
	x: s32
}
//#[swag.printbc]
#test
{
	pair := {"toto", Point(10)}
	@assert(pair.val1.x == 10)
}

