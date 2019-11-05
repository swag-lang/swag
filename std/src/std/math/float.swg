namespace std.math
{
    public
	{
        const Pi: f64       = 3.1415927410125732421875
        const Sqrt2: f64    = sqrt(2.0'f64)
    }
    
	#[swag.constexpr]
    public
	{
		func saturate(x: f32) => clamp(x, 0, 1)
		func saturate(x: f64) => clamp(x, 0, 1)
		func lerp(x, y, factor: f32) => ((1 - factor) * x) + (factor * y)
		func lerp(x, y, factor: f64) => ((1 - factor) * x) + (factor * y)
	}
	
	#[swag.constexpr]
    public
	{
		func cos(x: f32)->f32 => os.cosf(x)
		func cos(x: f64)->f64 => os.cos(x)
		func cosh(x: f32)->f32 => os.coshf(x)
		func cosh(x: f64)->f64 => os.cosh(x)
		func acos(x: f32)->f32 => os.acosf(x)
		func acos(x: f64)->f64 => os.acos(x)
		func sin(x: f32)->f32 => os.sinf(x)
		func sin(x: f64)->f64 => os.sin(x)
		func sinh(x: f32)->f32 => os.sinhf(x)
		func sinh(x: f64)->f64 => os.sinh(x)
		func asin(x: f32)->f32 => os.asinf(x)
		func asin(x: f64)->f64 => os.asin(x)
		func tan(x: f32)->f32 => os.tanf(x)
		func tan(x: f64)->f64 => os.tan(x)
		func tanh(x: f32)->f32 => os.tanhf(x)
		func tanh(x: f64)->f64 => os.tanh(x)
		func atan(x: f32)->f32 => os.atanf(x)
		func atan(x: f64)->f64 => os.atan(x)
		func log(x: f32)->f32 => os.logf(x)
		func log(x: f64)->f64 => os.log(x)
		func log2(x: f32)->f32 => os.log2f(x)
		func log2(x: f64)->f64 => os.log2(x)
		func log10(x: f32)->f32 => os.log10f(x)
		func log10(x: f64)->f64 => os.log10(x)
		func sqrt(x: f32)->f32 => os.sqrtf(x)
		func sqrt(x: f64)->f64 => os.sqrt(x)
		func ceil(x: f32)->f32 => os.ceilf(x)
		func ceil(x: f64)->f64 => os.ceil(x)		
		func floor(x: f32)->f32 => os.floorf(x)
		func floor(x: f64)->f64 => os.floor(x)		
		func round(x: f32)->f32 => os.roundf(x)
		func round(x: f64)->f64 => os.round(x)		
		func abs(x: f32)->f32 => os.fabs(x)
		func abs(x: f64)->f64 => os.abs(x)	
		func pow(x, exp: f32)->f32 => os.powf(x, exp)
		func pow(x, exp: f64)->f64 => os.pow(x, exp)		
	}
}