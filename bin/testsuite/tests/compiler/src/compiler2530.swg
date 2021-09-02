func toto()->s64
{
    @seterr(null)
    return 555
}

//#[Swag.PrintBc]
#test
{
    x := toto()
    @assert(@err == null)
}