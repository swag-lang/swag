func toto()->s64 throw
{
    throw "toto"
    return 555
}

//#[swag.printbc]
#test
{
    g := 0
    x := catch toto()
    @assert(@err != null)
}