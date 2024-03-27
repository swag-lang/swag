struct A
{
    fct: closure(*Self)
}

#test
{
    var v: A
    v.fct = func(s)
    {
    }
}
