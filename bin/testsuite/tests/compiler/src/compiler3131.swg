#test
{
    var a = 0
    #scope
    {
        a += 1
        if a == 2:
            break
        continue
    }

    @assert(a == 2)
}