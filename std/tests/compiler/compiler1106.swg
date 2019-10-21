struct(T = s32) Toto
{
	x: T
}

#test
{
	var pt: Toto
	@assert(pt.x is s32)
	var pt1: Toto!f32
	@assert(pt1.x is f32)
}