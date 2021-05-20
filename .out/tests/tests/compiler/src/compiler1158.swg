#[Swag.inline]
func min(x: s32)->s32
{
	return x + x
}

#test
{
	x := min(1)
}