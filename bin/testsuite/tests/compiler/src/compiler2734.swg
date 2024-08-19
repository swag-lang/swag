#global testerror "Err0398"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}