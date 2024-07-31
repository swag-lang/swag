#global testerror "Err0196"

#test
{
    var x = 0
    var z = 2
    z += 1
    @init(&x, z)
}