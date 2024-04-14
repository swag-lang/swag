//#global testerror "Err0104"
#test
{
    var x = 5
    var y =,ref &x
    var z = 6
    @assert(y == 5)
    y =,ref &z
    @assert(y == 6)
}