#global testerror "Err0315"
struct ErrToto {}

func toto()
{
    #if false:
        throw ErrToto{}
    #else:
        throw ErrToto{}
}

#test
{
    toto()
}
