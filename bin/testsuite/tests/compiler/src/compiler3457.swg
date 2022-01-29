func toto()->closure()->s32
{
    counter := 0
    f := @|counter|()->s32 {
        counter += 1
        return counter
    }

    return f
}

#test
{
    f := toto()
    @assert(f() == 1)
    @assert(f() == 2)
    @assert(f() == 3)
}