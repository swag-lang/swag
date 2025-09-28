#global testerror "Err0394"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}