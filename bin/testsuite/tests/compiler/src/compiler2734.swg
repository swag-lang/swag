#global testerror "Err0446"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}