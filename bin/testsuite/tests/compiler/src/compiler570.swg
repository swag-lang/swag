alias ret = { a, b: s32 }

func toto()->ret
{
    return {1, 2}
}

#test
{
    let a = toto()
    @assert(a.item0 == 1)
    @assert(a.item1 == 2)
}
