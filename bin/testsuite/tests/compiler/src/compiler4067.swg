#global testerror "Err0550"
#test
{
    var toto = [10, 20]
    var r = &toto
    var x = 0
    for r
    {
        x += 1
    }

    @print(x)
    @assert(x == 2)
}