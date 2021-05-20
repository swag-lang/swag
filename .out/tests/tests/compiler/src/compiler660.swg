

enum OT
{
	A = 1
	B
}

//#[Swag.printbc]
//#global testerror
#test
{
	var pt: [2] OT = @[OT.A, OT.B]
	@assert(pt[0] == OT.A)
	@assert(pt[1] == OT.B)
}
