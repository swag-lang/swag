//#[Swag.PrintBc]
#test
{
    {
        let x  = -1's8
        let y: s32 = cast(s32) x
        @assert(y == -1)
    }

    {
        let x  = -1's8
        let y: s64 = cast(s64) x
        @assert(y == -1)
    }

    {
        let x  = -1's16
        let y: s64 = cast(s64) x
        @assert(y == -1)
    }
}
