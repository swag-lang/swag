struct Err {}

func toto() throw
{
    throw Err{}
}

#test
{
    catch toto()
    @assert(@err == Err)
}
