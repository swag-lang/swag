

//#[Swag.PrintBc]
func(T) sum(x, y: T) -> T => x + y

#test
{
	@assert(sum'u32(1, 3) == 4)
	@assert(sum'f64(1.0, 3.0) == 4.0)
}