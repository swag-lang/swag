struct A
{
    fct: func(s32, bool)
}

#test
{
    var v: A
    v.fct = func(x, y) {
        a := y
    }
}
