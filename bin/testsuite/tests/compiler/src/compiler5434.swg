#global testerror "Err0602"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}