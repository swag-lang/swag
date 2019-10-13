#module module1029

//#[swag.printbc]
#test
{
	x := 10
	init(&x)(0)
	@assert(x == 0)
}