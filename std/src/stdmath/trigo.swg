namespace stdmath.win32
{
	#[swag.foreign("ucrtbase")]
	{
		func cosf(x: f32)->f32;
		//func cos(x: f64)->f64;
	}
}

namespace stdmath.trigo
{
	#[swag.public]
	{
		func cos(x: f32)->f32 => win32.cosf(x)
		//func cos(x: f64)->f64 => win32.cos(x)
	}
	
	#test
	{
		x := cos(1.0'f32)
		@print(cast(f64) x)
		@print("\n")
	}
}