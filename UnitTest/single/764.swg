#module module764

struct Point
{
	var x: s32 = 666
	var y: s32
}

//#[swag.printbc]
#test
{	
	cpt := Point()
	@assert(cpt.x == 666)
	@assert(cpt.y == 0)
}
