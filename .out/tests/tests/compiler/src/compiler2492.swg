//#[swag.printbc]
#test
{
    {
        var x: *~1 s32 = null
        @assert(x == null)
    }

    {
        var x: *~2 s32 = null
        @assert(x == null)
    }

    {
        var x: *~4 s32 = null
        @assert(x == null)
    }

    {
        var x: *~8 s32 = null
        @assert(x == null)
    }
}