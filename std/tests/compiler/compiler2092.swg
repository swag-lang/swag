//#[swag.printbc]
#test
{
    alias RR = AA
    x := RR's64{}
    @assert(@typeof(x.v) == s64)
}

struct(T) AA
{
    v: T
}
