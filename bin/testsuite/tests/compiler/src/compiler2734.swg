#global testerror

func toto() throw
{
    throw "toto"
}

#test
{
    assume assume toto()
}