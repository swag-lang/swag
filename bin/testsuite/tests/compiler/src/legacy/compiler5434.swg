#global testerror "Err0604"

struct A {}

func toto(x: A) {}

func titi()
{
    toto(null)
}
