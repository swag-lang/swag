#if swag.WINDOWS
{
    namespace std.os
    {
        #[swag.foreign("ucrtbase")]
        {
            func powf(x, exp: f32)->f32;
            func pow(x, exp: f64)->f64;
        }
    }
}