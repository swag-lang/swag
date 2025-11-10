struct A
{
    fct: func(s32, bool)->bool
}

#test
{
    var v: A
    v.fct = func(x, y)
    {
        var a = y
        return true
    }
}
