#test
{
    var toto = "10"
    var r = ref &toto
    var x = 0
    loop r
    {
        x += 1
    }

    @assert(x == 2)
}