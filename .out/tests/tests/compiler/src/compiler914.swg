

//#[Swag.printbc]
func(T) sum(x, y: *T) => x[0] + y[0]

//#[Swag.printbc]
#test
{
	a := 1
	b := 2
	@assert(sum(&a, &b) == 3)

	b = 10
	@assert(sum(&a, &b) == 11)

	c := 1.0
	d := 5.5
	@assert(sum(&c, &d) == 6.5)
}