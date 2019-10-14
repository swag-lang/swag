

func toto(x: {s32, s32})
{
	@assert(x.val0 == 10)
	@assert(x.val1 == 20)
}

//#[swag.printbc]
#test
{
	var aa = {10,20}
	toto(aa)
}
