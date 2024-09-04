#global testerror "Err0462"
#test
{
    var a = #run {
        for 1:
            return 66.0
        return
    }

    @assert(a == 66.0)
}