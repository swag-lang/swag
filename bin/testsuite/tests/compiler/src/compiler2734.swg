#global testerror "Err0503"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}