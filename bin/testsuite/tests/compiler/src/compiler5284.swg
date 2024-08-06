#global testerror "Err0160"
#test
{
    var a = 0
    a = 66
    @init(&a)
    //@assert(a == 0)
}