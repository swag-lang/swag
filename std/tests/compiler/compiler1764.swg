//#[swag.printbc]
#test
{
    {
        x := 66'f32
        @assert(cast(s8) x == 66)
        @assert(cast(u8) x == 66)
    }

    {
        x := 255'f32
        @assert(cast(s8) x == -1)
        @assert(cast(u8) x == 255)
    }
}