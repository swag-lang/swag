#global testerror "Err0590"

struct A
{
    fct: func(*Self)
}

#test
{
    var v: A
    v.fct = closure||(s) {}
}