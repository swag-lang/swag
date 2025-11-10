//#[Swag.PrintBc]
#test
{
    var x   = 666
    let val = @mkany(&x, s32)

    let value = cast(s32) val
    @assert(value == 666)
    @assert(#typeof(value) == s32)
}
