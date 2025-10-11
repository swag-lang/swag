struct point
{
    x:   s32 = 666
}

//#[Swag.PrintBc]
#test
{
    var arr: [2] point
    let ptr  = &arr[0]
    @assert(ptr[0].x == 666)
}
