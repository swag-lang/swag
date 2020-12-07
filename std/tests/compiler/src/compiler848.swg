

//#[swag.printbc]
#test
{
	a := autocast 1
	@assert(a == 1)
	@assert(@typeof(a) == @typeof(s32))
}