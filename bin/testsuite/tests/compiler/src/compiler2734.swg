#global testerror "Err0384"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}