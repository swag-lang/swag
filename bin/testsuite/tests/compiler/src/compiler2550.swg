#global testerror "Err0345"
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