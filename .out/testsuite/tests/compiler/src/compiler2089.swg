struct(T) AA
{
    v: {T, T}
}

//#[Swag.printbc]
#test
{
    var x: AA's64
    @assert(@sizeof(x.v) == 16)
    //@print(@typeof(x.v).name)

    var x1: AA'bool
    @assert(@sizeof(x1.v) == 2)
}
