namespace std.math
{
	#[swag.public, swag.constexpr]
	{
		func saturate(x: f32) => clamp(x, 0, 1)
		func saturate(x: f64) => clamp(x, 0, 1)
	}
}