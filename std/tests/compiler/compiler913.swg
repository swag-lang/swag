

//#[swag.printbc]
func(T) sum(x: T, y: u32) => x >> y

//#[swag.printbc]
#test
{
	@assert(sum(0b100, 1) == 0b10)
}