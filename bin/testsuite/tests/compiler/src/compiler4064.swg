#test
{
    var toto = 10
    let r    = #ref &toto
    var x    = 0
    for r
    {
        x += 1
    }

    @assert(x == 10)
}
