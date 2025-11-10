//#[Swag.PrintBc]
#test
{
    {
        let x = 66'f32
        @assert(cast(s8) x == 66)
        @assert(cast(u8) x == 66)
    }

    {
        let x = 255'f32
        //@assert(cast(s8) x == -1)
        @assert(cast(u8) x == 255)
    }

    {
        let x = -120's32
        @assert(cast(s8) x == -120)
    }

    {
        let x = -50's32
        @assert(cast(s16) x == -50)
    }

    {
        /*var x = -4
        @assert(x >> 1 == -2)
        var y = -4's64
        @assert(y >> 1 == -2)*/

    }
}
