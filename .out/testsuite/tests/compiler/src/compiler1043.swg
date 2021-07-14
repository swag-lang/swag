

func new()->Point => @{1, 2}

struct Point
{
	x, y: s32
}

//#[Swag.PrintBc]
#test
{
	/*pt := new()
	@assert(pt.x == 1)
	@assert(pt.y == 2)*/

	a := new().x
	@assert(a == 1)
	b := new().y
	@assert(b == 2)

	@assert(new().x == 1)
	@assert(new().y == 2)
}