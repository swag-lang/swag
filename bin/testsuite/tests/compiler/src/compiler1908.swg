//#[Swag.PrintBc]
#test
{
    var x = 666
    var ptr = @typeof(x)
    var val = @mkany(&x, ptr)

    var value = cast(s32) val
    @assert(value == 666)
    @assert(@kindof(value) == s32)
}