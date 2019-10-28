namespace win32
{
	#[swag.foreign("ucrtbase")]
	{
		func cosf(x: f32)->f32;
		func cos(x: f64)->f64;
		func acosf(x: f32)->f32;
		func acos(x: f64)->f64;		
		func sinf(x: f32)->f32;
		func sin(x: f64)->f64;		
		func asinf(x: f32)->f32;
		func asin(x: f64)->f64;			
		func tanf(x: f32)->f32;
		func tan(x: f64)->f64;			
		func atanf(x: f32)->f32;
		func atan(x: f64)->f64;			
	}
}

namespace std.math
{
	#[swag.public]
	{
		func cos(x: f32)->f32 => win32.cosf(x)
		func cos(x: f64)->f64 => win32.cos(x)
		func acos(x: f32)->f32 => win32.acosf(x)
		func acos(x: f64)->f64 => win32.acos(x)
		func sin(x: f32)->f32 => win32.sinf(x)
		func sin(x: f64)->f64 => win32.sin(x)
		func asin(x: f32)->f32 => win32.asinf(x)
		func asin(x: f64)->f64 => win32.asin(x)		
		func tan(x: f32)->f32 => win32.tanf(x)
		func tan(x: f64)->f64 => win32.tan(x)
		func atan(x: f32)->f32 => win32.atanf(x)
		func atan(x: f64)->f64 => win32.atan(x)		
	}
}