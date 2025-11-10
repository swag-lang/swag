//#[Swag.PrintBc]
#test
{
    {
        var x: u32 = 10'u8
        let y: u8 = 1
        x = y
    }

    {
        var x: f64 = 10
        let y: f32 = 1
        x = y
        @assert(x == 1)
    }
}
