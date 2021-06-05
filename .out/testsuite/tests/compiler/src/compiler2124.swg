struct(T) Arr
{
    struct ST1
    {
        x: T
    }

    struct ST
    {
        st: ST1
    }

    z: ST
}

//#[Swag.printbc]
#test
{
    var x: Arr'f64
    //@print(@typeof(x.z.x).name)
    @assert(@typeof(x.z.st.x) == f64)
}