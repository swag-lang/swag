#global testerror "Err0598"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}