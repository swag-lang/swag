namespace std.math
{
	#[swag.foreign("ucrtbase")]
	{
		func cosf(x: f32)->f32;
	}

	#test
	{
		x := cosf(1.0)
		@print(cast(f64) x)
	}
}