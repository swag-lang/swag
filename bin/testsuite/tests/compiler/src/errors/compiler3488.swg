#global testerror "Err0043"

struct A
{
    fct: func(*A)
}

#test
{
    var v: A
    v.fct = func||(s) {}
}
