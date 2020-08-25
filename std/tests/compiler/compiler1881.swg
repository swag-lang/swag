using swag

#[printbc]
#[optimizebc(1)]
#test
{
    a := 10
    b := a + 0
    @assert(b == a)
    b = 0 + a
    @assert(b == a)
}

#[optimizebc(1)]
#test
{
    a := 10
    b := a * 1
    @assert(b == a)
    b = 1 * a
    @assert(b == a)
    b = 0 * a
    @assert(b == 0)
    b = a * 0
    @assert(b == 0)
}