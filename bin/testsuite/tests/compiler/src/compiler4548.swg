#global testerror "Err0711"

enum A { R; G; B; }

func toto(x: A)
{
}

func titi()
{
    toto(.RR)
}
