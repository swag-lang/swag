func toto()->s64
{
    @seterr("toto")
    return 555
}

//#[swag.printbc]
#test
{
    x := toto() try
    @assert(false)
}