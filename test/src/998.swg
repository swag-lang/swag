#module module998
using swag

#test
{
	x := 6
	@assert(x is s32)
	@assert(6.0 is f32)
	@assert(6 is s32)
	@assert("6" is string)
}
