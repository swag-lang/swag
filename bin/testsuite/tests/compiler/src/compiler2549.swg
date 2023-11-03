struct ErrToto{}

#test
{
    toto()
}

func toto()
{
    titi()
#if false throw ErrToto{}
}

func titi()
{
#if false throw ErrToto{}
}