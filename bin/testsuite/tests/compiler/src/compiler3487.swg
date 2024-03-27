struct A
{
    fct: closure(*Self)
}

#test
{
    var v: A
    v.fct = closure||(s)
    {
    }
}
