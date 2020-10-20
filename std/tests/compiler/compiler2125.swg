struct(T) Arr
{
    struct ST1
    {
        x: T
    }

    z: ST1
}

//#[swag.printbc]
#test
{
    var x: Arr'f64
    @assert(@typeof(x.z.x) == f64)
}