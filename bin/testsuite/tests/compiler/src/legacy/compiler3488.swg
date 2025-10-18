#global testerror "Err0044"

struct A
{
    fct: func(*A)
}

#test
{
    var v: A
    v.fct = func||(s) {}
}
