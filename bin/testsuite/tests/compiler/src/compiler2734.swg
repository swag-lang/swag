#global testerror "Err0395"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}