#global testerror "Err0555"

#test
{
    var x = 0
    var z = 2
    z += 1
    @drop(&x, z)
}