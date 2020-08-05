#if swag.WINDOWS
{
    namespace std.os
    {
        #[swag.foreign("ucrtbase")]
        {
            func coshf(x: f32)->f32;
            func cosh(x: f64)->f64;
            func sinhf(x: f32)->f32;
            func sinh(x: f64)->f64;
            func tanhf(x: f32)->f32;
            func tanh(x: f64)->f64;
            func powf(x, exp: f32)->f32;
            func pow(x, exp: f64)->f64;
        }
    }
}