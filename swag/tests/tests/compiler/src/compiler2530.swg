func toto()->s64
{
    @seterr(null)
    return 555
}

//#[swag.printbc]
#test
{
    x := toto()
    @assert(@err == null)
}