#global testerror "Syn0140"

func toto() throw
{
    throw "toto"
}

#test
{
    assume assume toto()
}