func toto() throw
{
    throw "toto"
}

#test
{
    catch toto()
    @assert(@errmsg == "toto")
}