#[Swag.Optimize(.All, false)]
#test
{
    var x = 0's8
    var y = 0's8
    x += 6
    y += 3
    @assert(x / y == 2)
}

#[Swag.Optimize(.All, false)]
#test
{
    var x = 0's16
    var y = 0's16
    x += 6
    y += 3
    @assert(x / y == 2)
}

#[Swag.Optimize(.All, false)]
#test
{
    var x = 0'u8
    var y = 0'u8
    x += 6
    y += 3
    @assert(x / y == 2)
}

#[Swag.Optimize(.All, false)]
#test
{
    var x = 0'u16
    var y = 0'u16
    x += 6
    y += 3
    @assert(x / y == 2)
}
