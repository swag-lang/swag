struct(T) Arr
{
    z: ST
}

impl Arr
{
    struct ST
    {
        x, y: s32
    }
}

//#[Swag.PrintBc]
#test
{
    var x: Arr'f64
    @assert(#typeof(x.z.x) == s32)
    //@assert(#typeof(Arr.ST.x) == s32)
}