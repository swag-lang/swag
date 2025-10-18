#global testerror "Err0084"

enum A { R, G, B }

func toto(x: A) {}

func titi()
{
    toto(.RR)
}
