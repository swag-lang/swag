#global testerror "Err0579"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}