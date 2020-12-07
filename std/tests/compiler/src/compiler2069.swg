//#[swag.printbc]
#test
{
    {
        x := 2'f32
        x = @sqrt(x)
        //@print(cast(f64) x)
        @assert(1.4 < x < 1.42)
    }
    {
        x := 2'f64
        x = @sqrt(x)
        //@print(cast(f64) x)
        @assert(1.4 < x < 1.42)
    }
}