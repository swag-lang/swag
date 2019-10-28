namespace std.math
{
	#[swag.public, swag.constexpr]
	{
		func(T) clamp(x, min, max: T) => x < min ? min : (x > max ? max : x)
	}
}