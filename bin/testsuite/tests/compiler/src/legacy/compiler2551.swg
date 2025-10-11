//#global testerror
struct ErrToto {}

func(T) toto()
{
    #if false:
        throw ErrToto{}
}

#test
{
    toto's32()
}
