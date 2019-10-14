

func toto(x: {f32})
{
	@assert(x.val0 == 1.0)
}

//#[swag.printbc]
#test
{
	toto({1.0})
}