struct ErrToto {}

#test
{
    toto()
}

func toto()
{
    titi()
    #if false do
        throw ErrToto{}
}

func titi()
{
    #if false do
        throw ErrToto{}
}
