#[swag.inline]
func min(x, y: s32) => x < y ? x : y

//#[swag.printbc]
#test
{
	x := min(5, min(4, 2))
	@assert(x == 2)
}