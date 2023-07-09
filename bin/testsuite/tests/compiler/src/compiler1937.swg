//#[Swag.PrintBc]
#test
{
    var x = 1'f32
    var y = 1'f32
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    var x = 1'f64
    var y = 1'f64
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    var x = 1'f32
    var y = 1'f32
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    var x = 1'f64
    var y = 1'f64
    x += y
    x = x + 1
    @assert(x == 3)
}

#test
{
    var x = 3's32
    var y = -1's32
    x += y
    x = x / -1
    @assert(x == -2)
}
