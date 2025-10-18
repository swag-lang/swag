#global testerror "Err0547"
struct ErrToto {}

func toto()
{
    #if false do
        throw ErrToto{}
    #else do
        throw ErrToto{}
}

#test
{
    toto()
}
