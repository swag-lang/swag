func toto()->s64
{
    @seterr("toto")
    return 555
}

//#[swag.printbc]
#test
{
    x := try toto()
    @assert(false)
}