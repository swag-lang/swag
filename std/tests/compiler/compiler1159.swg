#[swag.inline]
func(T) min(x, y: T) => x < y ? x : y

//#[swag.printbc]
#test
{
	x := min(2, 1)
	@assert(x == 1)
}