#global testerror "Err0647"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}