

//#[swag.printbc]
#test
{
	var a: f32
	var b: s32 = 1
	a = autocast b
	@assert(a == 1)
}