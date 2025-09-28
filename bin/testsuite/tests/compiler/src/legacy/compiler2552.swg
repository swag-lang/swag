//#global testerror
struct ErrToto{}

func(T) toto()
{
#if false:
    #if false:
        throw ErrToto{}
}

#test
{
    toto's32()
}