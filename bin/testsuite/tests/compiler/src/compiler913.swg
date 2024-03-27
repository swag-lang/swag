

//#[Swag.PrintBc]
func(T) sum(x: T, y: u32) => x >> y

//#[Swag.PrintBc]
#test
{
	@assert(sum(0b100, 1) == 0b10)
}