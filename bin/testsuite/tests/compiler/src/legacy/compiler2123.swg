#global testerror "Err0024"

struct(T) Arr
{
    struct(T) ST { x, y: T }

    z:   ST
}

//#[Swag.PrintBc]
#test
{
    var x: Arr'f64
    //@print(#typeof(x.z.x).name)
    @assert(#typeof(x.z.x) == f64)
}
