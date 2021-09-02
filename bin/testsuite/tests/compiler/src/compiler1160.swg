#[Swag.Inline]
func min(x, y: s32) => x < y ? x : y

//#[Swag.PrintBc]
#test
{
	x := min(5, min(4, 2))
	@assert(x == 2)
}