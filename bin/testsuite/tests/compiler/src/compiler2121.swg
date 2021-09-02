struct(T) Arr
{
    z: ST's32
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
    @assert(@typeof(x.z.x) == s32)
    //@assert(@typeof(Arr.ST.x) == s32)
}