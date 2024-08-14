#global testerror "Err0511"

func toto()->closure(s32)
{
    var a = 666
    var v: closure(s32) = closure|a|(a, x: s32)
    {
        @assert(a == 666)
        @assert(x == 5)
    }

    return v
}

#test
{
    let f = toto()
    f(5)
}