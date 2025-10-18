#global testerror "Err0745"

struct A {}

func toto(x: A) {}

func titi()
{
    toto(null)
}
