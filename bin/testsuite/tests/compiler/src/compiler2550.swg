#global testerror "Err0443"
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