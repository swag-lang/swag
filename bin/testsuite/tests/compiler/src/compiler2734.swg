#global testerror "Err0425"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}