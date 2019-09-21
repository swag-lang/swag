#module module914

//#[swag.printbc]
func(T) sum(x, y: *T) => x[0] + y[0]

//#[swag.printbc]
#test
{
	a := 1
	b := 2
	@assert(sum(&a, &b) == 3)
	
	b = 10
	@assert(sum(&a, &b) == 11)
}