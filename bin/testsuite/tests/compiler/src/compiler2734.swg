#global testerror "Err0266"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}