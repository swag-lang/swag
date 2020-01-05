//#[swag.printbc]
func(T) sum(x, y: T) => x + y

//#[swag.printbc]
#test
{
	@assert(sum(1's32, 3) == 4)
	@assert(sum'f32(1.0, 3.0) == 4)
}