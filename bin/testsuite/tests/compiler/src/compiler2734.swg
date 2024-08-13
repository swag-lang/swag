#global testerror "Err0386"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}