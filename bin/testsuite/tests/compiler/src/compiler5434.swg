#global testerror "Err0549"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}