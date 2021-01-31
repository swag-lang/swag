//#[swag.printbc]
#test
{
    {
        #[swag.relative(1)]
        var x: *s32 = null
        @assert(x == null)
    }

    {
        #[swag.relative(2)]
        var x: *s32 = null
        @assert(x == null)
    }

    {
        #[swag.relative(4)]
        var x: *s32 = null
        @assert(x == null)
    }

    {
        #[swag.relative(8)]
        var x: *s32 = null
        @assert(x == null)
    }
}