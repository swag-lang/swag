#test
{
    var a = 0
    #scope
    {
        a += 1
        if a == 2 do
            break
        continue
    }

    @assert(a == 2)
}
