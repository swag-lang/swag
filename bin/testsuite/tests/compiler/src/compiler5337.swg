#global testerror "Err0643"
struct A {}
struct B {}

func toto(x: B) {}

#test
{
    var x: A
    x.toto()
}