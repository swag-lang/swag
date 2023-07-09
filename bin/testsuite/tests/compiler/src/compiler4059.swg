func toto()
{
    var toto = 0
    var toto1 = 1
    var toto2 = 2

    var ref1 = ref &toto1
    var ref2 = ref &toto2

    @assert(-ref1 == -1)
}

#test
{
    toto()
}