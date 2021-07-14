//#[Swag.PrintBc]
#test
{
    {
        var x: u32 = 10'u8
        var y: u8 = 1
        x = y
    }

    {
        var x: f64 = 10
        var y: f32 = 1
        x = y
        @assert(x == 1)
    }
}