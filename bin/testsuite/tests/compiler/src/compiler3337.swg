#global testerror "Err0146"

#test
{
    struct A {}
    struct B { vv: A; }
    var x: B
    x.Vv = 0
}