#global testerror "Err0297"

//#[Swag.PrintBc]
#test
{
    typealias RRRR = AAAA
    var x = RRRR's64{}
    @assert(@typeof(x.v) == s64)
}

struct(T) AAAA
{
    v: T
}
