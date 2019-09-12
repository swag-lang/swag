#module module819

func toto(x: {s32, s32})
{
	@assert(x.0 == 10)
	@assert(x.1 == 20)
}

//#[swag.printbc]
#test
{
	var aa = {10,20}
	toto(aa)
}
