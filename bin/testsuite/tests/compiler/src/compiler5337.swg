#global testerror "Err0646"
struct A {}
struct B {}

func toto(x: B) {}

#test
{
    var x: A
    x.toto()
}