#global testerror "Err0704"

enum A { R; G; B; }

func toto(x: A)
{
}

func titi()
{
    toto(.RR)
}
