struct U
{
	y: bool
	x: s32
}

union U1
{
	y: bool
	x: s32
}

#[swag.pack]
struct U2
{
	y: bool
	x: s32
}

#test
{
	@assert(@sizeof(U) == 8)
	@assert(@sizeof(U1) == 4)
	@assert(@sizeof(U2) == 5)
}
