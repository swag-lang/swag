#global testerror "Err0700"

#test
{
    struct A {}
    struct B { vv: A; }
    var x: B
    x.Vv = 0
}