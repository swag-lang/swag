struct A
{
    x:   s32 = 555
}

//#[Swag.PrintBc]
#test
{
    var x: [2] A

    x[0].x = 1
    x[1].x = 2

    var count = 1
    count -= 1
    @init(&x[0], count)

    @assert(x[0].x == 1)
    @assert(x[1].x == 2)
}
