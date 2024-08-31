#global testerror "Err0347"
struct ErrToto{}

func toto()
{
#if false: throw ErrToto{}
#else: throw ErrToto{}
}

#test
{
    toto()
}