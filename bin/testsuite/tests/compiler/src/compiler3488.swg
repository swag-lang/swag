#global testerror "Err0638"

struct A
{
    fct: func(*Self)
}

#test
{
    var v: A
    v.fct = closure||(s) {}
}