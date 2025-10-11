//#[Swag.PrintBc]
#test
{
    //var array = {1,2,3}
    //F(cast([..]s32) array, &T)
    var c = [666, 667]
    F(cast([..] s32) c, &T)
    @assert(c[0] == 667)
    @assert(c[1] == 668)
}

//#[Swag.PrintBc]
func T(param: *s32)
{
    dref param += 1
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
