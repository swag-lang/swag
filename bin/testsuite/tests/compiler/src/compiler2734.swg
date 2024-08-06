#global testerror "Err0480"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}