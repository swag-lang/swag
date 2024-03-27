func toto(x: ...)
{
    visit str, i: x
    {
        switch i
        {
        case 0:
            @assert(cast(string) str == "a")
        case 1:
            @assert(cast(string) str == "bb")
        case 2:
            @assert(cast(s32) str == 666)
        }
    }
}

#test
{
    toto("a", "bb", 666)
}
