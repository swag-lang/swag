struct U
{
	y: bool
	x: s32
}

#[Swag.Pack(0)]
struct U1
{
	y: bool
	x: s32
}

#[Swag.Pack(1)]
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
