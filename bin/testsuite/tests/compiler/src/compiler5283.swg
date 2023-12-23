//#global skip
//#global testerror "Err0787"
#test
{
    var a = 0
    a = 66
    @init(a)
    @assert(a == 0)
}