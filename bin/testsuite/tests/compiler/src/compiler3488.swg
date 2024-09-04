#global testerror "Err0515"

struct A
{
    fct: func(*Self)
}

#test
{
    var v: A
    v.fct = func||(s) {}
}