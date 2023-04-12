#[Swag.Optim("", false)]
#test
{
    x := 0's8
    y := 0's8
    x += 2
    y += 3
    @assert(x * y == 6)
}

#[Swag.Optim("", false)]
#test
{
    x := 0's16
    y := 0's16
    x += 2
    y += 3
    @assert(x * y == 6)
}

#[Swag.Optim("", false)]
#test
{
    x := 0'u8
    y := 0'u8
    x += 2
    y += 3
    @assert(x * y == 6)
}

#[Swag.Optim("", false)]
#test
{
    x := 0'u16
    y := 0'u16
    x += 2
    y += 3
    @assert(x * y == 6)
}
