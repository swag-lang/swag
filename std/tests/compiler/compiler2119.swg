struct(T) Arr
{
    struct ST
    {
        x, y: T
    }

    z: ST
}

//#[swag.printbc]
#test
{
    var x: Arr's32
    @print(@typeof(x.z.x).name)
    //@assert(@typeof(Arr.ST.x) == s32)
}