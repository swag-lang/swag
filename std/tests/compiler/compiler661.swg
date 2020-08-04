enum OT
{
	A = 1
	B
}

//#[swag.printbc]
//#unittest error
#test
{
	pt := @{x: OT.A}
}
