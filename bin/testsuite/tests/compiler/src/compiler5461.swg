struct(T) A
{
    b: bool
    x: T
}

var a: A's16

#test
{
    a.x = 1

    a.x *= 2
    @assert(a.x == 2)
    a.x += 2
    @assert(a.x == 4)
    a.x -= 1
    @assert(a.x == 3)
}