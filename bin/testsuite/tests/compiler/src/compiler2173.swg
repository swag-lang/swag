func toto(x: ...)
{
    foreach str, i in x
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
        //@print(str)
    }
}

#test
{
    toto("a", "bb", 666)
}
