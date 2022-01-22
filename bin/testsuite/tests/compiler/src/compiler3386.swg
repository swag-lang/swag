struct A
{
    fct: func(s32, bool)
}

#test
{
    var v: A
    v.fct = @(x, y) {
        a := y
    }
}
