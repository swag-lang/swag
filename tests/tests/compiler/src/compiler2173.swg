func toto(x: ...)
{
    visit str, i: x
    {
        switch i
        {
            case 0: @assert(str == "a")
            case 1: @assert(str == "bb")
            case 2: @assert(str == 666)
        }
        //@print(str)
    }
}

#test
{
    toto("a", "bb", 666)
}