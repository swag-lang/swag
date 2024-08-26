#global testerror "Err0641"
#test
{
    var a = #run {
        for 1:
            return 66.0
        return true
    }

    @assert(a == 66.0)
}