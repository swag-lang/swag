//#[Swag.printbc]
#test
{
    x := 1'f32
    y := 1'f32
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    x := 1'f64
    y := 1'f64
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    x := 1'f32
    y := 1'f32
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    x := 1'f64
    y := 1'f64
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    x := 3's32
    y := -1's32
    x += y
    x = x / -1
    @assert(x == -2)
}
