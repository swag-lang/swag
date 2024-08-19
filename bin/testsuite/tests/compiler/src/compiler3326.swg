#global testerror "Err0634"
#test
{
    var a = #run {
        loop 1:
            return 66.0
        return true
    }

    @assert(a == 66.0)
}