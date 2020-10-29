#module "compiler2123"
#unittest error

struct(T) Arr
{
    struct(T) ST
    {
        x, y: T
    }

    z: ST
}

//#[swag.printbc]
#test
{
    var x: Arr'f64
    //@print(@typeof(x.z.x).name)
    @assert(@typeof(x.z.x) == f64)
}