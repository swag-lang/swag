


enum OT
{
	A = 1
	B
}

struct Toto
{
	x: OT = OT.A
}

//#[swag.printbc]
#test
{
	var pt: Toto
	@assert(pt.x == 1)
	@assert(pt.x == OT.A)
}
