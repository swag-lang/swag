//#[Swag.printbc]
#test
{
    a := @[1, 2, 3, 4]
    c := a[1..3]
    d := c[1..2]
    @assert(d[0] == 3)
    @assert(@countof(d) == 2)
}