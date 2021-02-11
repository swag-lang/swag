func toto()->s64
{
    @seterr(null)
    return 555
}

//#[swag.printbc]
#test
{
    x := catch toto()
    @assert(@err == null)
}