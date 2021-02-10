#test
{
    @seterr("toto")
    @assert(@err == "toto")
    @seterr(null)
}