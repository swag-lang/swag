enum EN: s32
{
    Red   = 666
    Green
}

//#[Swag.PrintBc]
#test
{
    using EN
    let val = Red
    @assert(val == 666)
}
