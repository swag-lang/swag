#global testerror "Err0594"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}