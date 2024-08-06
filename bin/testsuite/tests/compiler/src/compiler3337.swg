#global testerror "Err0710"

#test
{
    struct A {}
    struct B { vv: A; }
    var x: B
    x.Vv = 0
}