#[Swag.Optim("", false)]
#test
{
    var x = 0's8
    var y = 0's8
    x += 2
    y += 3
    @assert(x * y == 6)
}

#[Swag.Optim("", false)]
#test
{
    var x = 0's16
    var y = 0's16
    x += 2
    y += 3
    @assert(x * y == 6)
}

#[Swag.Optim("", false)]
#test
{
    var x = 0'u8
    var y = 0'u8
    x += 2
    y += 3
    @assert(x * y == 6)
}

#[Swag.Optim("", false)]
#test
{
    var x = 0'u16
    var y = 0'u16
    x += 2
    y += 3
    @assert(x * y == 6)
}
