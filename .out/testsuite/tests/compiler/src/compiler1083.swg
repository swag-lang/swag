#[Swag.pack(0)]
struct Union
{
	x: s32
	y: bool
}

#test
{
	var t: Union
	t.y = true
	@assert(t.x == 1)
}
