#global testerror "Err0444"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}