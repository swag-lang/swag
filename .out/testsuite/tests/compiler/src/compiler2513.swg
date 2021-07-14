func toto(x: string)-> [100] u8
{
    var result: retval
    len := @countof(x)
    @memcpy(@dataof(result), @dataof(x), len)
    result[len] = 0
    return result
}

//#[Swag.PrintBc]
#test
{
    x := toto("toto")
    @assert(@cstrlen(@dataof(x)) == 4)
}