func toto()->closure()->s32
{
    var counter = 0
    var f = closure|counter|()->s32 {
        counter += 1
        return counter
    }

    return f
}

#test
{
    var f = toto()
    @assert(f() == 1)
    @assert(f() == 2)
    @assert(f() == 3)
}