struct(T) AA 
{
    v: T
}

//#[swag.printbc]
#test
{
    alias RR = AA
    x := RR's64{}
    @assert(@typeof(x.v) == s64)
}
