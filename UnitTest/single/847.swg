#module module847

//#[swag.printbc]
#test
{
	var a: f32
	var b: s32 = 1
	a = cast b
	@assert(a == 1)
}