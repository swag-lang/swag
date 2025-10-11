//#[Swag.PrintBc]
#test
{
    var x: func(a: s32 = 1)
    x = func(a)
    {
        @assert(a == 1)
    }

    x()
    x(1)

    x = func(a)
    {
        @assert(a == 2)
    }

    x(2)
}
