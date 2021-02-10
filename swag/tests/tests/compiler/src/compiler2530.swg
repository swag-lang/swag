func toto()->s64
{
    @seterr(null)
    return 555
}

//#[swag.printbc]
#test
{
    g := 0
    x := toto() catch g = 1
    @assert(g == 0)
}