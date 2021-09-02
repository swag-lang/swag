

//#[Swag.PrintBc]
func(T) sum(x, y: T)->T => x + y

//#[Swag.PrintBc]
#test
{
	@assert(sum'u32(1, 3) == 4)
	@assert(sum(1.0'f32, 3.0) == 4)
}