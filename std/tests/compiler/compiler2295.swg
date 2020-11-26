func xx(b: s64...)
{
    @assert(@countof(b) == 4)
    @assert(b[0] == 10)
    @assert(b[1] == 20)
    @assert(b[2] == 30)
    @assert(b[3] == 40)
}

//#[swag.printbc]
#test
{
    var arr: [4] s64 = @[10, 20, 30, 40]
    xx(@spread(arr))
}