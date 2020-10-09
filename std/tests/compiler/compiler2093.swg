func(T) AA()
{
}

//#[swag.printbc]
#test
{
    alias RR = AA's32
    RR()
    //@assert(@typeof(x.v) == s64)
}
