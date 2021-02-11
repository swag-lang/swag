func toto()->s64
{
    @seterr("toto")
    return 555
}

//#[swag.printbc]
#test
{
    g := 0
    x := catch toto()
    @assert(@err != null)
}