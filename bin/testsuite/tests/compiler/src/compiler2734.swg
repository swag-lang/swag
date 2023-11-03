#global testerror "Err1147"

func toto() throw
{
    throw ErrToto{}
}

#test
{
    assume assume toto()
}