func toto()->{ v: s32 }
{
    var result: retval
    result.v = 10
    return result
}

#test
{
    alias TT = func()->{ a: s32 }
    var t: TT
    t = &toto
}
