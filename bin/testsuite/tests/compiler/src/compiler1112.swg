#test
{
	let x = 10000 / cast(f32) 0xFFFFFFFF
	let y = 10000 / cast(f32) Swag.U32.Max
	@assert(x == y)
	//@print(cast(f64) x)
}
