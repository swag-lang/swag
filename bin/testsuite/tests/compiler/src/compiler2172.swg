func toto(x: string...)
{
    visit str, i: x
    {
        switch i
        {
        case 0:
            @assert(str == "a")
        case 1:
            @assert(str == "bb")
        case 2:
            @assert(str == "ccc")
        }
    }
}

#test
{
    toto("a", "bb", "ccc")
}
