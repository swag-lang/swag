#module module673
//#unittest pass syntax

//#[swag.printbc]
#test
{
	var a : {s32, s32}
	a.val0 = 1
	a.val1 = 2
	@assert(a.val0 == 1)
	@assert(a.val1 == 2)
}
