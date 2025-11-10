var g = 0

//#[Swag.PrintBc]
#test
{
    g = 0

    var array = [1, 2, 3]
    F(cast([..] s32) array, &T)
    @assert(g == 6)
}

func T(param: *s32)
{
    g += dref param
}

//#[Swag.PrintBc]
func F(param: [..] s32, b: func(*s32))
{
    let data = @dataof(param)
    for param
    {
        b(data + @index)
    }
}
