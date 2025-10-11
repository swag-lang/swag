#test
{
    //var x: func(x: s32 = 1, y: s32 = 1)
    let x = func(a, b: s32 = 1)
    {
        @assert(a == 1 and b == 1)
    }

    x()
}
