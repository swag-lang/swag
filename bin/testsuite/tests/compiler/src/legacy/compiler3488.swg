#global testerror "Err0527"

struct A
{
    fct: func(*A)
}

#test
{
    var v: A
    v.fct = func||(s) {}
}