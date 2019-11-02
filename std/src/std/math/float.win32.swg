#if WINDOWS
{
    namespace std.os
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
}