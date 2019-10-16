#module compiler507
#unittest pass semantic

//#[swag.printbc]
#unittest error
#test
{
	a := [0,1,2]
	b := a[-1]
}
