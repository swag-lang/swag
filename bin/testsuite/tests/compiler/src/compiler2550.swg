#global testerror "Err0324"
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