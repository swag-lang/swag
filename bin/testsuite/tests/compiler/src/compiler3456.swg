#test
{
    let counter = 0
    let f       = func|counter|()->s32
    {
        counter += 1
        return counter
    }

    @assert(f() == 1)
    @assert(f() == 2)
    @assert(f() == 3)
}
