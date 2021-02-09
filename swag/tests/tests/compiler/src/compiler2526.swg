#global testerror

//#[swag.haserror]
#test
{
    @seterr("toto")
    @assert(@err == "toto")
    @seterr(null)
}