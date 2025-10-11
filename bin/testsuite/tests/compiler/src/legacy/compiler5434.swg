#global testerror "Err0561"

struct A {}

func toto(x: A) {}

func titi()
{
    toto(null)
}
