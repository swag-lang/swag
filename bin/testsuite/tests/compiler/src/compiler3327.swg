//#global skip
#global testerror "Err0779"
#test
{
    var a = #run {
        loop 1
            return 66.0
        return
    }

    @assert(a == 66.0)
}