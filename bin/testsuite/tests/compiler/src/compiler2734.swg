#global testerror "Err0491"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}