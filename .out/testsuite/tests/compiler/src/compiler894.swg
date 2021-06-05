func toto(x: {f32})
{
	@assert(x.item0 == 1.0)
}

//#[Swag.printbc]
#test
{
	toto(@{1.0})
}