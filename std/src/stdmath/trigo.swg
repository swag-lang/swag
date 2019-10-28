namespace stdmath.win32
{
	#[swag.foreign("ucrtbase")]
	{
		func cosf(x: f32)->f32;
		func cos(x: f64)->f64;
		func sinf(x: f32)->f32;
		func sin(x: f64)->f64;		
	}
}

namespace stdmath.trigo
{
	#[swag.public]
	{
		func cos(x: f32)->f32 => win32.cosf(x)
		func cos(x: f64)->f64 => win32.cos(x)
		func sin(x: f32)->f32 => win32.sinf(x)
		func sin(x: f64)->f64 => win32.sin(x)
	}
}