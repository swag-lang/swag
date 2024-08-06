#global testerror "Err0637"

struct A {}

func toto(x: A)
{
}

func titi()
{
    toto(null)
}