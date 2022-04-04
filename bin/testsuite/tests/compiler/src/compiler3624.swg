#global testerror

enum E { A; }
enum F { B; }

func toto(v: F)
{
}

#test
{
    x := E.A
    switch x
    {
    case A: toto(.C)
    }
}