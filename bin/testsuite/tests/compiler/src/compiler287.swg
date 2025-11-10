//#[Swag.PrintBc]
#test
{
    var a: bool = true
    let b  = &a
    let c  = dref b
    @assert(c == true)
}

#test
{
    var a: s32 = 666
    let b  = &a
    let c  = dref b
    @assert(c == 666)
}
