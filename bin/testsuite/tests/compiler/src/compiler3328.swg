#global testerror "Err0591"
#test
{
    var a = #run {
        for 1:
            return
        return true
    }

    @assert(a == 66.0)
}