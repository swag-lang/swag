#global testerror "Err0420"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}