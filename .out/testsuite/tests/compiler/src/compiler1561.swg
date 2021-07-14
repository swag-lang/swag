struct(T) A
{
}

//#[Swag.PrintBc]
#test
{
    var x: A's32
    @assert(@typeof(x).name == "__compiler1561.A'(s32)")
}