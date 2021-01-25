//#[swag.printbc]
#test
{
    {
        x := -1's8
        var y: s32 = cast(s32) x
        @assert(y == -1)
    }

    {
        x := -1's8
        var y: s64 = cast(s64) x
        @assert(y == -1)
    }
}