//#[Swag.printbc]
#test
{
    x := 666
    ptr := @typeof(x)
    val := @mkany(&x, ptr)

    value := cast(s32) val
    @assert(value == 666)
    @assert(@kindof(value) == s32)
}