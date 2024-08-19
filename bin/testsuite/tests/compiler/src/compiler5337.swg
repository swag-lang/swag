#global testerror "Err0570"
struct A {}
struct B {}

func toto(x: B) {}

#test
{
    var x: A
    x.toto()
}