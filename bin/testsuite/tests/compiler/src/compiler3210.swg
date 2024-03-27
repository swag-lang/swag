#global testerror "Err0649"
struct A
{
}
struct B
{
    using a: A
}

func toto(x: *A)
{
}

func titi()
{
    var a = B{}
    toto(a)
}
