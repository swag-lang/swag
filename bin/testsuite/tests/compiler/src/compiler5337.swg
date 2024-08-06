#global testerror "Err0605"
struct A {}
struct B {}

func toto(x: B) {}

#test
{
    var x: A
    x.toto()
}