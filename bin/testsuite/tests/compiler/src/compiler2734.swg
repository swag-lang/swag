#global testerror "Err0445"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}