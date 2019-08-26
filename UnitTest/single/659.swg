#module module659
//#unittest pass syntax

enum OT
{
	A = 1
	B
}

//#[swag.printbc]
#unittest error
#test
{
	var pt: [1] OT
}
