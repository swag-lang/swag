public func toto3594()
{
    var g = 0
    if ((true and true and true) or
       (false and false and false))
    {
        g = 1
    }
    @assert(g == 1)

    if (true and true and true) or
       (false and false and false)
    {
        g = 2
    }
    @assert(g == 2)
}

#test
{
    toto3594()
}
