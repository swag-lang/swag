#global testerror "Err0489"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}