#global testerror "Err0715"

enum A { R; G; B; }

func toto(x: A)
{
}

func titi()
{
    toto(.RR)
}
