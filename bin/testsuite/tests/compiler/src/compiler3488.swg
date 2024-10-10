#global testerror "Err0527"

struct A
{
    fct: func(*Self)
}

#test
{
    var v: A
    v.fct = func||(s) {}
}