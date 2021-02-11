func toto()->s64
{
    throw "toto"
    return 555
}

//#[swag.printbc]
#test
{
    x := try toto()
    @assert(false)
}