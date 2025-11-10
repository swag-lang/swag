enum EN: s32
{
    Red   = 666
    Green
}

//#[Swag.PrintBc]
#test
{
    using EN
    let val: EN = Red
    @assert(val == 666)
}
