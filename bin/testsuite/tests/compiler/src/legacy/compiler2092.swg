#global testerror "Err0120"

//#[Swag.PrintBc]
#test
{
    alias RRRR = AAAA
    var x = RRRR's64{}
    @assert(#typeof(x.v) == s64)
}

struct(T) AAAA
{
    v:   T
}
