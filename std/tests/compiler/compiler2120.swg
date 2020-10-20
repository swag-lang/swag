struct(T) Arr
{
    z: ST'T
}

impl Arr
{
    struct(T) ST
    {
        x, y: T
    }
}

//#[swag.printbc]
#test
{
    var x: Arr'f64
    @assert(@typeof(x.z.x) == f64)
    //@assert(@typeof(Arr.ST.x) == s32)
}