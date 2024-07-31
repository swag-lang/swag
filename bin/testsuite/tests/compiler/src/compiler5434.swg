#global testerror "Err0656"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}