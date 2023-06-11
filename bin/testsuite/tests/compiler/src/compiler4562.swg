func toto() throw
{
    throw "toto"
}

#test
{
    catch toto()
    @assert(@errmsg == "toto")
    @assert(@err().msg == "toto")
}