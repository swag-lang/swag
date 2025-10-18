#global testerror "Err0289"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}