//#global skip
//#global testerror "Err0557"
#test
{
    var a = 0
    a = 66
    @init(a)
    @assert(a == 0)
}