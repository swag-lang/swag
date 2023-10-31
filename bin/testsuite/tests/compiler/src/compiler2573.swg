//#global testerror

func toto() throw
{
    throw "titi"
}

func titi() throw
{
    catch toto()
    if cast(string) @err() throw "super titi"
}

#test
{
    catch titi()
    @assert(@err() != null)
}