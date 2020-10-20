struct(T) Arr
{
    z: ST'T
}

impl Arr
{
    struct(T1) ST
    {
        x, y: T1
    }
}

//#[swag.printbc]
#test
{
    var x: Arr's32
    @assert(@typeof(x.z.x) == s32)
    //@assert(@typeof(Arr.ST.x) == s32)
}