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

#test
{
	@assert(@sizeof(U) == 8)
	@assert(@sizeof(U1) == 4)
}
