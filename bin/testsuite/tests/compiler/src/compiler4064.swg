#test
{
    var toto = 10
    let r =,ref &toto
    var x = 0
    loop r
    {
        x += 1
    }

    @assert(x == 10)
}