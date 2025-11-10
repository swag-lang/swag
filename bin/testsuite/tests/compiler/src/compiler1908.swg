//#[Swag.PrintBc]
#test
{
    var x   = 666
    let ptr = #typeof(x)
    let val = @mkany(&x, ptr)

    let value = cast(s32) val
    @assert(value == 666)
    @assert(#typeof(value) == s32)
}
