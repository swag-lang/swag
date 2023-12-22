#global testerror "Err0138"
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