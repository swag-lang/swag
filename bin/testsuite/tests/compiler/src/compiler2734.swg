#global testerror "Err0379"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}