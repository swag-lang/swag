#global testerror "Err0304"
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