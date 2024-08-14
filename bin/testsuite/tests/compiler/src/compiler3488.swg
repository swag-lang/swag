#global testerror "Err0549"

struct A
{
    fct: func(*Self)
}

#test
{
    var v: A
    v.fct = closure||(s) {}
}