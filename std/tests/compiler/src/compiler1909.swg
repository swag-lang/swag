//#[swag.printbc]
#test
{
    x := 666
    val := @mkany(&x, s32)

    value := cast(s32) val
    @assert(value == 666)
    @assert(@kindof(value) == s32)
}