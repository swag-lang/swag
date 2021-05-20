func toto()->s64
{
    @seterr(null)
    return 555
}

//#[Swag.printbc]
#test
{
    x := toto()
    @assert(@err == null)
}