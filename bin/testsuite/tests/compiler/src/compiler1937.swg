//#[Swag.PrintBc]
#test
{
    var x = 1'f32
    let y = 1'f32
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    var x = 1'f64
    let y = 1'f64
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    var x = 1'f32
    let y = 1'f32
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    var x = 1'f64
    let y = 1'f64
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    var x = 3's32
    let y = -1's32
    x += y
    x = x / -1
    @assert(x == -2)
}
