func toto() => 666
#test
{
    var g = 0
    if let x = toto()
    {
        g = x
    }

    @assert(g == 666)
}
