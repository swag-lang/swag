func toto(y: s8)->s8
{
    var x = y
    x *= 2
    return x
}

#test
{
    let y = toto(2)
    @assert(y == 4)
}

func toto1(y: s16)->s16
{
    var x = y
    x *= 2
    return x
}

#test
{
    let y = toto1(3)
    @assert(y == 6)
}
