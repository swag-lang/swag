#global testerror "Err0639"

struct A
{
    fct: func(*Self)
}

#test
{
    var v: A
    v.fct = closure||(s) {}
}