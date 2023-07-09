//#[Swag.PrintBc]
#test
{
    var a = 1's64
    var b = 2's64
    var c = a > b
    @assert(c == false)
    var c1 = b > a
    @assert(c1 == true)
    var d = a < b
    @assert(d == true)
    var d1 = b < a
    @assert(d1 == false)
}

//#[Swag.PrintBc]
#test
{
    var a = 1'f32
    var b = 2'f32
    var c = a > b
    @assert(c == false)
    var c1 = b > a
    @assert(c1 == true)
    var d = a < b
    @assert(d == true)
    var d1 = b < a
    @assert(d1 == false)
}

//#[Swag.PrintBc]
#test
{
    var a = 1'f64
    var b = 2'f64
    var c = a > b
    @assert(c == false)
    var c1 = b > a
    @assert(c1 == true)
    var d = a < b
    @assert(d == true)
    var d1 = b < a
    @assert(d1 == false)
}