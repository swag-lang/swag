func toto()->func||()->s32
{
    let counter = 0
    let f       = func|counter|()->s32
    {
        counter += 1
        return counter
    }

    return f
}

#test
{
    let f = toto()
    @assert(f() == 1)
    @assert(f() == 2)
    @assert(f() == 3)
}
