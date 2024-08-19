#global testerror "Err0591"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}