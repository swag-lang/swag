struct(T) Arr
{
    z: ST'T
}

impl Arr
{
    struct ST
    {
        x, y: T
    }
}

//#[Swag.PrintBc]
#test
{
    var x: Arr'f64
    @assert(#typeof(x.z.x) == f64)
    //@assert(#typeof(Arr.ST.x) == s32)
}