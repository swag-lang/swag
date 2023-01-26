#test
{
	var y: u32 = 0xFFFFFFFF
	var x: s32 = cast,bit(s32) y
	var z: s64 = cast(s64) x
	@assert(z == -1)
}
