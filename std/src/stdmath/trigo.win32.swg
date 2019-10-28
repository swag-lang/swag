namespace win32
{
	#[swag.foreign("ucrtbase")]
	{
		func cosf(x: f32)->f32;
		func cos(x: f64)->f64;
		func coshf(x: f32)->f32;
		func cosh(x: f64)->f64;
		func acosf(x: f32)->f32;
		func acos(x: f64)->f64;
		func sinf(x: f32)->f32;
		func sin(x: f64)->f64;
		func sinhf(x: f32)->f32;
		func sinh(x: f64)->f64;
		func asinf(x: f32)->f32;
		func asin(x: f64)->f64;
		func tanf(x: f32)->f32;
		func tan(x: f64)->f64;
		func tanhf(x: f32)->f32;
		func tanh(x: f64)->f64;
		func atanf(x: f32)->f32;
		func atan(x: f64)->f64;
		func logf(x: f32)->f32;
		func log(x: f64)->f64;
		func log2f(x: f32)->f32;
		func log2(x: f64)->f64;
		func log10f(x: f32)->f32;
		func log10(x: f64)->f64;
		func sqrtf(x: f32)->f32;
		func sqrt(x: f64)->f64;
		func ceilf(x: f32)->f32;
		func ceil(x: f64)->f64;		
		func floorf(x: f32)->f32;
		func floor(x: f64)->f64;
		func roundf(x: f32)->f32;
		func round(x: f64)->f64;		
		func powf(x, exp: f32)->f32;
		func pow(x, exp: f64)->f64;			
	}
}

namespace std.math
{
	#[swag.public, swag.constexpr]
	{
		func cos(x: f32)->f32 => win32.cosf(x)
		func cos(x: f64)->f64 => win32.cos(x)
		func cosh(x: f32)->f32 => win32.coshf(x)
		func cosh(x: f64)->f64 => win32.cosh(x)
		func acos(x: f32)->f32 => win32.acosf(x)
		func acos(x: f64)->f64 => win32.acos(x)
		func sin(x: f32)->f32 => win32.sinf(x)
		func sin(x: f64)->f64 => win32.sin(x)
		func sinh(x: f32)->f32 => win32.sinhf(x)
		func sinh(x: f64)->f64 => win32.sinh(x)
		func asin(x: f32)->f32 => win32.asinf(x)
		func asin(x: f64)->f64 => win32.asin(x)
		func tan(x: f32)->f32 => win32.tanf(x)
		func tan(x: f64)->f64 => win32.tan(x)
		func tanh(x: f32)->f32 => win32.tanhf(x)
		func tanh(x: f64)->f64 => win32.tanh(x)
		func atan(x: f32)->f32 => win32.atanf(x)
		func atan(x: f64)->f64 => win32.atan(x)
		func log(x: f32)->f32 => win32.logf(x)
		func log(x: f64)->f64 => win32.log(x)
		func log2(x: f32)->f32 => win32.log2f(x)
		func log2(x: f64)->f64 => win32.log2(x)
		func log10(x: f32)->f32 => win32.log10f(x)
		func log10(x: f64)->f64 => win32.log10(x)
		func sqrt(x: f32)->f32 => win32.sqrtf(x)
		func sqrt(x: f64)->f64 => win32.sqrt(x)
		func ceil(x: f32)->f32 => win32.ceilf(x)
		func ceil(x: f64)->f64 => win32.ceil(x)		
		func floor(x: f32)->f32 => win32.floorf(x)
		func floor(x: f64)->f64 => win32.floor(x)		
		func round(x: f32)->f32 => win32.roundf(x)
		func round(x: f64)->f64 => win32.round(x)		
		func pow(x, exp: f32)->f32 => win32.powf(x, exp)
		func pow(x, exp: f64)->f64 => win32.pow(x, exp)		
	}
}