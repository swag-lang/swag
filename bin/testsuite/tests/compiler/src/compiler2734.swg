#global testerror "Err0383"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}