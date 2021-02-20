#global testerror

func toto() throw
{
    throw "titi"
}

func titi() throw
{
    catch toto()
    if @err throw "super titi"
}

#test
{
    try titi()
}