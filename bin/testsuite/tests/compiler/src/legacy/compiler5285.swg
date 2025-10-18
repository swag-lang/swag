//#global testerror "Err0476"
#test
{
    var a = [1, 2, 3]
    @assert(a[2] == 3)
    @init(a)
    @assert(a[2] == 0)
    //@assert(a == 0)
}
