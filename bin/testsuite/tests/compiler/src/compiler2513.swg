func toto(x: string)-> [100] u8
{
    var result: retval
    var len = @countof(x)
    @memcpy(@dataof(result), @dataof(x), len)
    result[len] = 0
    return result
}

//#[Swag.PrintBc]
#test
{
    var x = toto("toto")
    @assert(@strlen(@dataof(x)) == 4)
}