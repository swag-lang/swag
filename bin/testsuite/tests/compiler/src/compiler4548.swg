#global testerror "Err0698"

enum A { R; G; B; }

func toto(x: A)
{
}

func titi()
{
    toto(.RR)
}
