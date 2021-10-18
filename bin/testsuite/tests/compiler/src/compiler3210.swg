#global testerror
struct A {}
struct B
{
    using a: A
}

func toto(x: *A)
{
}

func titi()
{
    a := B{}
    toto(a)
}