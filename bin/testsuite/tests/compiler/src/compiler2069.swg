//#[Swag.PrintBc]
#test
{
    {
        var x = 2'f32
        x = @sqrt(x)
        //@print(cast(f64) x)
        @assert(1.4 < x and x < 1.42)
    }
    {
        var x = 2'f64
        x = @sqrt(x)
        //@print(cast(f64) x)
        @assert(1.4 < x and x < 1.42)
    }
}