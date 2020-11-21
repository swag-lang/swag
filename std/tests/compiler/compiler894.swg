func toto(x: {f32})
{
	@assert(x.item0 == 1.0)
}

//#[swag.printbc]
#test
{
	toto(@{1.0})
}