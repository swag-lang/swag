func toto()->s64 throw
{
    throw "toto"
    return 555
}

//#[Swag.PrintBc]
#test
{
    g := 0
    x := catch toto()
    @assert(@err != null)
}