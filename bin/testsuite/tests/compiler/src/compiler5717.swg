#global testerror "Err0318"
//#global skip
//#[Swag.PrintBc]
#test
{
    var x = 666
    const ptr = #typeof(x)
    let val = @mkany(&x, ptr)

    let value = cast(s32) val
    @assert(value == 666)
    @assert(@kindof(value) == s32)
}