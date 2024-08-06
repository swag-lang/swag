#global testerror "Err0436"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}