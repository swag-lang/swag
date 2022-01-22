struct A
{
    fct: func(s32, bool)->bool
}

#test
{
    var v: A
    v.fct = @(x, y) {
        a := y
        return true
    }
}
