//#[Swag.PrintBc]
#test
{
    {
        x := 66'f32
        @assert(cast(s8) x == 66)
        @assert(cast(u8) x == 66)
    }

    {
        x := 255'f32
        //@assert(cast(s8) x == -1)
        @assert(cast(u8) x == 255)
    }

    {
        x := -120's32
        @assert(cast(s8) x == -120)
    }

    {
        x := -50's32
        @assert(cast(s16) x == -50)
    }

    {
        /*x := -4
        @assert(x >> 1 == -2)
        y := -4's64
        @assert(y >> 1 == -2)*/

    }
}