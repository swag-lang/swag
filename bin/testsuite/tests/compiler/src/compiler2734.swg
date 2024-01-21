#global testerror "Err0500"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}