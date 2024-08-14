#global testerror "Err0583"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}