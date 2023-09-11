#global testerror "Syn0147"

func toto() throw
{
    throw "toto"
}

#test
{
    assume assume toto()
}