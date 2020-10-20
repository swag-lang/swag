struct(T) Arr
{
    struct(T) ST1
    {
        x: T
    }

    struct(T) ST
    {
        st: ST1
    }

    z: ST
}

//#[swag.printbc]
#test
{
    var x: Arr'f64
    //@print(@typeof(x.z.x).name)
    @assert(@typeof(x.z.st.x) == f64)
}