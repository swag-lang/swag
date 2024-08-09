#global testerror "Err0394"
struct ErrToto{}

func toto()
{
#if false #do throw ErrToto{}
#else #do throw ErrToto{}
}

#test
{
    toto()
}