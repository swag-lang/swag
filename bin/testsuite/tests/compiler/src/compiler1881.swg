using Swag

//#[PrintBc]
#test
{
    var a = 10
    var b = a + 0
    @assert(b == a)
    b = 0 + a
    @assert(b == a)
    b = a - 0
    @assert(b == a)
    b = a / 1
    @assert(b == a)
}

#test
{
    var a = 10
    var b = a * 1
    @assert(b == a)
    b = 1 * a
    @assert(b == a)
    b = 0 * a
    @assert(b == 0)
    b = a * 0
    @assert(b == 0)
}

#test
{
    var a = 1
    if false and a > 0
        @assert(true)
    if a > 0 and false
        @assert(true)
}