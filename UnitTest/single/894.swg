#module module894

func toto(x: {f32})
{
	@assert(x.0 == 1.0)
}

//#[swag.printbc]
#test
{
	toto({1})
}