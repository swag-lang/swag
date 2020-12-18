struct(T) A
{
}

//#[swag.printbc]
#test
{
    var x: A's32
    @assert(@typeof(x).name == "__compiler1561.A'(s32)")
}