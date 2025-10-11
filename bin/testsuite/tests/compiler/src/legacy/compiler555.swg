struct point
{
    x:   s32 = 666
}

//#[Swag.PrintBc]
#test
{
    var arr: point
    let ptr  = &arr
    @assert(ptr.x == 666)
}
