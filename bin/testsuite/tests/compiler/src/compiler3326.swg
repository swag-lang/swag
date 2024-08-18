#global testerror "Err0626"
#test
{
    var a = #run {
        loop 1:
            return 66.0
        return true
    }

    @assert(a == 66.0)
}