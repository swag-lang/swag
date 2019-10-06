#module module1041

//#[swag.printbc]
#test
{
	var pt : s32 = 1
	ptr := &pt
	drop(ptr)
	@assert(pt == 1)
}