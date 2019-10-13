

//#[swag.printbc]
func(T) sum(x, y: T) -> T => x + y

#test
{
	@assert(sum(1, 3) == 4)
	@assert(sum(1.0, 3.0) == 4.0)
}