#global testerror "Err0597"

struct A
{
    fct: func(*Self)
}

#test
{
    var v: A
    v.fct = closure||(s) {}
}