


enum OT
{
	A = 1
	B
}

struct Toto
{
	x: OT = OT.A
}

//#[Swag.PrintBc]
#test
{
	var pt: Toto
	@assert(pt.x == 1)
	@assert(pt.x == OT.A)
}
