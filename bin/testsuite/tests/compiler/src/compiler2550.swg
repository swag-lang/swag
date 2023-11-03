#global testerror "Err0138"
struct ErrToto{}

func toto()
{
#if false throw ErrToto{}
#else throw ErrToto{}
}

#test
{
    toto()
}