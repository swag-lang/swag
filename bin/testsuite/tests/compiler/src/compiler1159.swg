#[Swag.Inline]
func(T) min(x, y: T) => x < y ? x : y

//#[Swag.PrintBc]
#test
{
	let x = min'f32(2, 1)
	@assert(x == 1)
}
