func toto() throw
{
    throw "toto"
}

#test
{
    catch toto()
    @assert(cast(string) @err() == "toto")
}