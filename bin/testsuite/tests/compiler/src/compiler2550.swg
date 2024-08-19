#global testerror "Err0321"
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