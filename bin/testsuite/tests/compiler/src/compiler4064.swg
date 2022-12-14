#test
{
    toto := 10
    r := ref &toto
    x := 0
    loop r
    {
        x += 1
    }

    @assert(x == 10)
}