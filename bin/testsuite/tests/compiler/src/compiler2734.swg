#global testerror "Err1147"

func toto() throw
{
    throw "toto"
}

#test
{
    assume assume toto()
}