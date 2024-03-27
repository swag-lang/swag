//#global testerror "Err0199"
#test
{
    var a = [1, 2, 3]
    @assert(a[2] == 3)
    @init(a)
    @assert(a[2] == 0)
}
