#global testerror "Err0707"

enum A { R; G; B; }

func toto(x: A)
{
}

func titi()
{
    toto(.RR)
}
