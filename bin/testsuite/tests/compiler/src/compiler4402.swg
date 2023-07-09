func toto(t: typeinfo) {}

func(T) titi(x: T)
{
    toto(T)
}

//#[Swag.PrintBc]
#test
{
    var x = func() {}

    titi(x)
}