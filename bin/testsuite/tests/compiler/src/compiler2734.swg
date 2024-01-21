#global testerror "Err0501"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}