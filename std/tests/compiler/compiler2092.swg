//#[swag.printbc]
#test
{
    alias RRRR = AAAA
    x := RRRR's64{}
    @assert(@typeof(x.v) == s64)
}

struct(T) AAAA
{
    v: T
}
