

//#[Swag.printbc]
func(T) sum(x: T, y: u32) => x >> y

//#[Swag.printbc]
#test
{
	@assert(sum(0b100, 1) == 0b10)
}