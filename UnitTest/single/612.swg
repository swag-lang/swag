#module module612
//#unittest pass semantic

#unittest error
//#[swag.printbc]
#test
{
	x := 0
	@assert(x == 0)
	x := 0
}