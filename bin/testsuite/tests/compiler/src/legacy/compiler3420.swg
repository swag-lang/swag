#global testerror "Err0184"

func toto()->func||(s32)
{
    var a  = 666
    var v: func||(s32) = func|a, a|(x: s32)
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
