//#global testerror

#test
{
    @seterr("toto")
    @assert(@err == "toto")
    @seterr(null)
}