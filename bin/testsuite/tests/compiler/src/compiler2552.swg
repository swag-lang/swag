//#global testerror
struct ErrToto{}

func(T) toto()
{
#if false #do
    #if false #do
        throw ErrToto{}
}

#test
{
    toto's32()
}