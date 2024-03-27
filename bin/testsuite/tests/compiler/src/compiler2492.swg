#global skip // no more relative pointers (18 july 2021)
//#[Swag.PrintBc]
#test
{
    {
        var x: *~8 s32 = null
        @assert(x == null)
    }

    {
        var x: *~16 s32 = null
        @assert(x == null)
    }

    {
        var x: *~32 s32 = null
        @assert(x == null)
    }

    {
        var x: *~64 s32 = null
        @assert(x == null)
    }
}