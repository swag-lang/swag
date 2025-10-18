#global testerror "Err0084"

enum E { A }
enum F { B }

func toto(v: F) {}

#test
{
    var x = E.A
    switch x
    {
    case A:
        toto(.C)
    }
}
