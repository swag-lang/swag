#global testerror "Err0649"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}