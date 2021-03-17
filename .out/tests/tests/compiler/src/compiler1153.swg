#[swag.inline]
func toto(x, y: s64)
{
	@assert(x + y == 10)
}

//#[swag.printbc]
#test
{
	toto(3, 7)
	toto(5, 5)
	
	x := 9's64
	y := 1's64
	toto(x, y)
}